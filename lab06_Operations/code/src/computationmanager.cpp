//     ____  __________     ___   ____ ___  _____ //
//    / __ \/ ____/ __ \   |__ \ / __ \__ \|__  / //
//   / /_/ / /   / / / /   __/ // / / /_/ / /_ <  //
//  / ____/ /___/ /_/ /   / __// /_/ / __/___/ /  //
// /_/    \____/\____/   /____/\____/____/____/   //
// Auteurs : Edwin Haeffner, Arthur Junod
// Date : 28.01.2024
// Fichier : computationmanager.cpp


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou méthodes pour vous aider
// déclarez les dans ComputationManager.h et définissez les méthodes ici.
// Certaines fonctions ci-dessous ont déjà un peu de code, il est à remplacer, il est là temporairement
// afin de faire attendre les threads appelants et aussi afin que le code compile.

#include <algorithm>
#include "computationmanager.h"


ComputationManager::ComputationManager(int maxQueueSize): MAX_TOLERATED_QUEUE_SIZE(maxQueueSize)
{
}

/**
 * Allow a client to request a computation to our calculators
 *
 * @param c Computation we want to compute
 * @return the id of the computation requested
 */
int ComputationManager::requestComputation(Computation c) {

    monitorIn();

    ComputationType type = c.computationType;

    // If the queue is full, we wait
    if(!stopped && bufferComputation[type].size() >= MAX_TOLERATED_QUEUE_SIZE){
        wait(waitFullComputation[type]);
    }

    if(stopped){
        monitorOut();
        signal(waitFullComputation[type]);
        throwStopException();
    }


    bufferComputation[type].emplace_back(c, nextId);
    signal(waitEmptyComputation[type]);
    waitingIds.push_back(nextId);
    monitorOut();
    return nextId++;
}

/**
 * Allow us to abort a computation by giving its id
 *
 * @param id The id of the computation we want to abort
 */
void ComputationManager::abortComputation(int id) {

    monitorIn();

    if(stopped){
        monitorOut();
        return;
    }

    // Lambda to find the id in the buffer of answers
    auto itAns = std::find_if(bufferAnswers.begin(), bufferAnswers.end(), [id](const Result& r) {
        return r.getId() == id;
    });

    //Remove answer if the id has been found in the list of answers
    auto itFound = itAns;
    if(itFound != bufferAnswers.end()){
        bufferAnswers.erase(itFound);
        signal(waitAnswer);
    }

    int i = 0;
    // Search and remove from bufferComputation if it hasn't started
    for(auto &buffer : bufferComputation) {
        auto itComp = std::find_if(buffer.second.begin(), buffer.second.end(), [id](const Request& r) { return r.getId() == id; });
        if(itComp != buffer.second.end()) {
            buffer.second.erase(itComp);
            signal(waitFullComputation[(ComputationType) i]);
            break;
        }
        ++i;
    }


    //Remove from the wait buffer
    waitingIds.remove(id);
    monitorOut();
}

/**
 * The clients call this method to get the result of a computation
 * If the earliest computation is not finished, the client waits
 * @return The result of the computation with the lowest id
 */
Result ComputationManager::getNextResult() {

    monitorIn();

    // We check if the earliest request is finished, if not we wait
    while(!stopped && (bufferAnswers.empty() || bufferAnswers.begin()->getId() != waitingIds.front())){
        wait(waitAnswer);
    }

    if(stopped){
        monitorOut();
        throwStopException();
    }

    Result r = *bufferAnswers.begin();
    bufferAnswers.erase(bufferAnswers.begin());
    waitingIds.pop_front();
    monitorOut();
    return r;
}

/**
 * The calculators call this method to get a computation to do
 * @param computationType The type of computation we want to do (A, B or C)
 * @return The request that has to be computed
 */
Request ComputationManager::getWork(ComputationType computationType) {

    monitorIn();
    if(!stopped && bufferComputation[computationType].empty()){
        wait(waitEmptyComputation[computationType]);
    }

    if(stopped){
        monitorOut();
        signal(waitEmptyComputation[computationType]);
        throwStopException();
    }

    Request r = bufferComputation[computationType].front();
    bufferComputation[computationType].pop_front();

    signal(waitFullComputation[computationType]);
    monitorOut();
    return r;
}

/**
 * Verify that the computation a calculator works on is still needed and wasn't aborted
 *
 * @param id The id of the computation the calculator works on
 * @return If we have to continue to work on it
 */
bool ComputationManager::continueWork(int id) {
    bool res = false;
    monitorIn();
    if (!stopped && std::find_if(waitingIds.begin(), waitingIds.end(), [id](const int& idWait) {
        return id == idWait;
    }) != waitingIds.end()) {
        res = true;
    }
    monitorOut();
    return res;
}

/**
 * Add a result to the bufferAnswers
 *
 * @param result Result to add
 */
void ComputationManager::provideResult(Result result) {
    monitorIn();
    bufferAnswers.insert(result);
    signal(waitAnswer);
    monitorOut();
}

/**
 * Stops everything !
 */
void ComputationManager::stop() {

    monitorIn();

    stopped = true;
    // We signal EVERYTHING, DEBOUT Là DEDANS
    signal(waitAnswer);
    for(auto &b : bufferComputation) {
        signal(waitEmptyComputation[b.first]);
        signal(waitFullComputation[b.first]);
    }

    monitorOut();

}
