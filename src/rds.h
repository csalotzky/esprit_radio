#ifndef Rds_h
#define Rds_h

#define PS_CACHE_SIZE 4
#define PS_MAX_WEIGHTSUM 64

#include "TEF668X/TEF668X.h"

class Rds {
  public:
    /* FUNCTIONS */
    Rds();
    void Reset();
    void ReceiveData(RdsData data);
    bool ReadPi(uint16_t *result, bool *realible);
    bool ReadEcc(uint8_t *result);
    bool ReadPs(char *result, bool *realible);
    bool ReadPty(uint8_t *result);
    bool ReadRt(char *result);
    bool ReadAf(uint8_t *result[]);

  private:
    /* FUNCTIONS */
    void ProcessPi(uint16_t blockAB_A_or_blockB_C, uint8_t errorAB_A_or_errorB_C);
    void ProcessEcc(uint16_t block1A_C, uint8_t error1A_C);
    void ProcessPs(uint16_t block0AB_B, uint16_t block0AB_D, uint8_t error0AB_BD);
    void ProcessPty(uint16_t blockAB_B, uint8_t errorAB_B);
    void ProcessRtA(uint16_t block2A_B, uint16_t block2A_C, uint16_t block2A_D, uint8_t error2A_BCD);
    void ProcessRtB(uint16_t block2B_B, uint16_t block2B_D, uint8_t error2B_BD);
    void ProcessAf(uint16_t block0A_C, uint8_t error0A_C);

    uint8_t Safe8bitAdd(uint8_t num, uint8_t add);


    /* STRUCTS */
    struct PiStruct {
      uint16_t Value;
      bool Available;
      bool IsRealible;
      
      uint8_t AverageSum;
      double AverageRatios[16];
    };

    struct EccStruct {
      uint8_t Value;
      bool Available;
    };

    struct PsChar {
      char Low;
      char High;
      uint8_t Weight; //0: not initialized, 1: 3 err, 2: 2 err, 8: 1 err, 255: 0 err
    };

    struct PsStruct {
      char Value[9];
      bool Available;
      bool IsRealible;

      uint8_t LastSelect; //last position to decide whether new line has started

      uint8_t CacheIndex;
      PsChar Cache[PS_CACHE_SIZE][4];   // store 0, 1 errors

      uint8_t AverageSum[4];      // weights
      float AverageRatios[4][16];   //-1.00 1.00 * 

    };

    struct PtyStruct {
      uint8_t Value;
      bool Available;
    };

    struct RtStruct {
      char Value[65];
      bool Available;

      bool IsA;
      bool Ready[16];
    };

    struct AfStruct {
      uint8_t Value;
      bool Available;
    };

    /* VARS */
    PiStruct Pi;
    EccStruct Ecc;
    PsStruct Ps;
    PtyStruct Pty;
    RtStruct Rt;
    AfStruct Af;
};

#endif