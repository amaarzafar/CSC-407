/*--------------------------------------------------------------------------*
 *----                                  ----*
 *----      joey.c                          ----*
 *----                                  ----*
 *----      This program implements the joey processes, which try to    ----*
 *----  leave the mall.                         ----*
 *----                                  ----*
 *----  ----    ----    ----    ----    ----    ----    ----    ----    ----*
 *----                                  ----*
 *----  Version 1.0     2017 April 17       Joseph Phillips ----*
 *----                                  ----*
 *--------------------------------------------------------------------------*/

//
//      Header inclusion:
//
#include    "kangarooHeaders.h"


//
//      Definition of global vars:
//

//  PURPOSE:  To tell if the mama is still looking for joeys.
int shouldStillRun = 1;

//  PURPOSE:  To tell which joey this joey is.
int joeyIndex;

//  PURPOSE:  To tell the process id of the mall process.
pid_t mallPid;

//  PURPOSE:  To hold what the next turn will be.
int nextTurn;

//  PURPOSE:  To initialize most globals given the 'argc' command line
//  arguments pointed to by 'argv[]'.  No return value.
void initializeMostGlobals(int argc, char*      argv[])
{
    //  I.  Application validity check:
    if  (argc < MIN_JOEY_CMD_LINE_PARAMS)
    {
        fprintf(stderr,"Usage: %s <index> <mallPid> <randSeed>\n", JOEY_PROG_NAME);
        exit(EXIT_FAILURE);
    }

    //  II.  Initialize globals:
    //  II.A.  Initialize 'joeyIndex':
    joeyIndex = atoi(argv[JOEYS_INDEX_CMD_LINE_INDEX]);

    //  II.B.  Initialize 'mallPid':
    mallPid = atoi(argv[JOEYS_MALL_PID_CMD_LINE_INDEX]);

    //  II.C.  Initialize random number generator:
    int seed = atoi(argv[JOEYS_RAND_SEED_CMD_LINE_INDEX]);

    srand(seed);

    //  III.  Finished:
}


//  PURPOSE:  To handle 'CORRECT_TURN_SIGNAL'.  Ignores 'sig'.  No return value.
void correctTurnSignalHandler(int sig)
{
    //  I.  Application validity check:

    //  II.
    nextTurn = ((rand() % 2) == 1) ? LEFT_TURN_SIGNAL : RIGHT_TURN_SIGNAL;
    printf("Joey # %d: Yay!  On to the next turn; I'll guess %s.\n",
           joeyIndex,
           (nextTurn == LEFT_TURN_SIGNAL) ? "left" : "right");
    fflush(stdout);

    kill(mallPid, nextTurn);

    //  III.  Finished:
}


//  PURPOSE:  To handle 'WRONG_TURN_SIGNAL'.  Ignores 'sig'.  No return value.
void wrongTurnSignalHandler(int sig)
{
    //  I.  Application validity check:

    //  II.
    nextTurn = (nextTurn == LEFT_TURN_SIGNAL)
               ? RIGHT_TURN_SIGNAL
               : LEFT_TURN_SIGNAL;
    printf("Joey # %d: Oops!  Let us try the other direction: %s.\n",
           joeyIndex,
           (nextTurn == LEFT_TURN_SIGNAL) ? "left" : "right");
    fflush(stdout);

    kill(mallPid, nextTurn);

    //  III.  Finished:
}


//  PURPOSE:  To note that the program may now finish (after receiving
//  'SIGINT').  Ignores 'sig'.   No return value.
void sigIntHandler(int sig)
{
    printf("Joey # %d: Yay!  I'm safely in mommy's pouch!\n", joeyIndex);
    fflush(stdout);
    shouldStillRun    = 0;
}


//  PURPOSE:  To install 'correctTurnSignalHandler()' as the signal handler for
//  'CORRECT_TURN_SIGNAL' and 'wrongTurnSignalHandler()' as the handler for
//  'WRONG_TURN_SIGNAL'.  No parameters.  No return value.
void installTurnHandlers()
{
    //  I.  Application validity check:

    //  II.  Install the handler:
    struct sigaction sigAction;
    memset(&sigAction, 0, sizeof(sigAction));

    //sigset_t   set;
    //sigemptyset(&set);
    //sigaddset(&set, CORRECT_TURN_SIGNAL);
    //sigaddset(&set, WRONG_TURN_SIGNAL);
    //sigAction.sa_mask = set;
    //sigAction.sa_flags = SA_RESTART;

    sigAction.sa_handler = correctTurnSignalHandler;
    sigaction(CORRECT_TURN_SIGNAL, &sigAction, 0);

    sigAction.sa_handler = correctTurnSignalHandler;
    sigaction(WRONG_TURN_SIGNAL, &sigAction, 0);

    //  III.  Finished:
}


//  PURPOSE:  To install 'sigIntHandler()' as the handler for 'SIGINT'.  No
//  parameters.  No return value.
void installSigIntHandler()
{
    //  I.  Application validity check:

    //  II.  Install handler:
    //  II.A.  Set up struct to specify the new action.
    // install your SIGINT handler
    struct sigaction sigAction;
    memset(&sigAction, 0, sizeof(sigAction));
    // sigset_t   set;
    //sigemptyset(&set);
    //sigaddset(&set, SIGINT);
    //sigAction.sa_mask = set;

    sigAction.sa_handler = sigIntHandler;
    sigaction(SIGINT, &sigAction, 0);

    //  III.  Finished:
}


int main (int argc, char* argv[])
{
    //  I.  Application validity check (done by 'initializeMostGlobals()'):

    //  II.  Try to leave mall:
    //  II.A.  Initialize globals:
    initializeMostGlobals(argc, argv);
    installTurnHandlers();
    installSigIntHandler();

    //  II.B.  Try to leave mall:
    nextTurn = ((rand() % 2) == 1) ? LEFT_TURN_SIGNAL : RIGHT_TURN_SIGNAL;
    printf("Joey # %d: Which direction?  Decisions, Decisions!  I'll guess %s.\n",
           joeyIndex,
           (nextTurn == LEFT_TURN_SIGNAL) ? "left" : "right");
    fflush(stdout);

    kill(mallPid, nextTurn);

    while  (shouldStillRun)
        sleep(1);

    //  III.  Finished:
    return(EXIT_SUCCESS);
}
