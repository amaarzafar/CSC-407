/*--------------------------------------------------------------------------*
 *----                                  ----*
 *----      mall.c                          ----*
 *----                                  ----*
 *----      This program implements the mall process, which gives the   ----*
 *----  joeys processes feedback on how to leave.           ----*
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

//  PURPOSE:  To hold the number of joeys the user requested to make.
int numJoeys = MIN_NUM_LEGAL_JOEYS - 1;

//  PURPOSE:  To tell if the mama is still looking for joeys.
int shouldStillRun = 1;

//  PURPOSE:  To encode the unique sequence of left and right turns each joey
//  make to leave the mall.
//  0 bit = left
//  1 bit = right
//  Bits read from least to most significant.
int* joeysPaths = NULL;

//  PURPOSE:  To keep track of the number of successful turns each joey has
//  already made.
int* joeysNumSuccessfulTurns = NULL;

//  PURPOSE:  To hold the process ids of the joey processes.
pid_t* joeysPids = NULL;


//
//      Definition of global fncs:
//

//  PURPOSE:  To initialize most global structures given the 'argc' command
//  line parameters given in 'argv[]'.  No return value.
void initializeMostGlobals(int argc, char* argv[])
{
    //  I.  Application validity check:
    if (argc < MIN_MALL_CMD_LINE_PARAMS)
    {
        fprintf(stderr,"Usage: %s <numJoeys> <randSeed>\n", MALL_PROG_NAME);
        exit(EXIT_FAILURE);
    }

    //  II.  Initialize globals:
    //  II.A.  Initialize 'numJoeys':
    numJoeys = atoi(argv[MALLS_NUM_JOEYS_CMD_LINE_INDEX]);

    if (numJoeys < MIN_NUM_LEGAL_JOEYS)
    {
        fprintf(stderr,"Illegal number of joeys.\n");
        exit(EXIT_FAILURE);
    }

    //  II.B.  Initialize random number seed:
    int   randNumSeed = atoi(argv[MALLS_RAND_SEED_CMD_LINE_INDEX]);

    srand(randNumSeed);

    //  II.C.  Initialize 'joeysPaths' and 'joeysNumSuccessfulTurns':
    joeysNumSuccessfulTurns = (int*) calloc(sizeof(int), numJoeys);
    joeysPaths = (int*) calloc(sizeof(int), numJoeys);
    joeysPids = (pid_t*) calloc(sizeof(pid_t), numJoeys);

    int numChoices = (1 << NUM_TURNS_MUST_SUCCESSFULLY_MAKE);
    int joey;

    for (joey = 0;  joey < numJoeys;  joey++)
        joeysPaths[joey] = rand() % numChoices;

    //  III.  Finished:
}


//  PURPOSE:  To return the index of the joey process 'joeyPid' in array
//      'joeysPids[]'.
int getJoeyIndex(int joeyPid)
{
    //  I.  Application validity check:

    //  II.  Find index of/for 'joeyPid':
    int   joey;

    //  II.A.  Look for 'joeyPid' in 'joeysPids[]':
    for (joey = 0; joey < numJoeys; joey++)
        if  ((joeysPids[joey] == joeyPid) || (joeysPids[joey] == 0))
            break;

    //  II.B.  Complain if run out of array positions:
    if  (joey >= numJoeys)
    {
        fprintf(stderr,
                "More joey Pids that number of joeys given on command line!\n");

        exit(EXIT_FAILURE);
    }

    //  II.C.  Record 'joeyPid' if not currently in 'joeysPids[]':
    if (joeysPids[joey] != joeyPid)
        joeysPids[joey] = joeyPid;

    //  III.  Finished:
    return(joey);
}


//  PURPOSE:  To return '1' ('true') if the joey 'joeyIndex' made the correct
//      turn with turn signal 'turnSignal', or to return '0' ('false')
//  otherwise.
int wasJoeysTurnCorrect(int joeyIndex, int turnSignal)
{
    //  I.  Application validity check:

    //  II.  Return value:
    int didJoeyTurnRight = (turnSignal == RIGHT_TURN_SIGNAL);

    int shouldHaveTurnedRight =
        (joeysPaths[joeyIndex] >> joeysNumSuccessfulTurns[joeyIndex]) & 0x1;

    return(didJoeyTurnRight == shouldHaveTurnedRight);
}

// Advanced signal handler
void turnSignalHandler(int sig, siginfo_t *siginfo, void *context)
{
    pid_t joeyProcessId = siginfo->si_pid;
    int joeyIndex = getJoeyIndex(joeyProcessId);
    char *dir = (sig == LEFT_TURN_SIGNAL) ? "left" : "right";

    if (!wasJoeysTurnCorrect(joeyIndex, sig))
    {
        printf("Mall: Joey # %i made the wrong turn!  (%s)\n",
               joeyIndex, dir);
               fflush(stdout);

               kill(joeyProcessId, WRONG_TURN_SIGNAL);
    }
    else
    {
        printf("Mall: Joey # %i turned correctly!  (%s)\n",
               joeyIndex, dir);
        fflush(stdout);

        ++joeysNumSuccessfulTurns[joeyIndex];

        if (joeysNumSuccessfulTurns[joeyIndex] >= NUM_TURNS_MUST_SUCCESSFULLY_MAKE)
        {
            printf("Mall: Joey # %i got out of the mall and returned to its mama!\n",
                   joeyIndex);
            fflush(stdout);

            kill(joeyProcessId, SIGINT);
        }
        else
        {
            printf("Mall: Joey # %i must make %i more correct turns.!\n",
                   joeyIndex,
                   NUM_TURNS_MUST_SUCCESSFULLY_MAKE -joeysNumSuccessfulTurns[joeyIndex]);
            fflush(stdout);

            kill(joeyProcessId, CORRECT_TURN_SIGNAL);
        }
    }
}


//  It printf()s a message and changes the global state so that the
//  while-loop in main() quits.
void sigIntHandler(int sig)
{
    printf("Mall: All the joeys have found their mama!  Now the mall can finally close!\n");
    fflush(stdout);
    shouldStillRun = 0;
}

//  PURPOSE:  To do the program given the 'argc' command line parameters
//  pointed to by 'argv[]'.  Returns 'EXIT_SUCCESS' to OS when program
//  naturally finishes.
int main(int argc, char* argv[])
{
    //  I.  Application validity check (done by 'initializeMostGlobals()'):

    //  II.  Sheppard joeys out of mall:
    //  II.A.  Initialize:
    initializeMostGlobals(argc, argv);

    // install LEFT_TURN_SIGNAL and RIGHT_TURN_SIGNAL handler
    struct sigaction sigAction;
    memset(&sigAction, 0, sizeof(sigAction));

    //sigset_t   set;
    //sigemptyset(&set);
    //sigaddset(&set, LEFT_TURN_SIGNAL);
    //sigaddset(&set, RIGHT_TURN_SIGNAL);
    //sigAction.sa_mask = set;
    sigAction.sa_flags = SA_SIGINFO /*| SA_RESTART*/;

    sigAction.sa_sigaction = turnSignalHandler;
    sigaction(LEFT_TURN_SIGNAL, &sigAction, 0);
    sigaction(RIGHT_TURN_SIGNAL, &sigAction, 0);

    // install your SIGINT handler
    memset(&sigAction, 0, sizeof(sigAction));
    //sigemptyset(&set);
    //sigaddset(&set, SIGINT);
    //sigAction.sa_mask = set;
    sigAction.sa_handler = sigIntHandler;
    sigaction(SIGINT, &sigAction, 0);

    //  II.B.  Sheppard joeys out of mall:
    while  (shouldStillRun)
        sleep(1);

    //  II.C.  Clean up afterward:
    free(joeysPids);
    free(joeysNumSuccessfulTurns);
    free(joeysPaths);

    //  III.  Finished:
    return(EXIT_SUCCESS);
}
