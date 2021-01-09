#include "rds.h"

/* 
    http://www.g.laroche.free.fr/english/rds/groupes/listeGroupesRDS.htm

            0...   A  ...15     0...   B  ...15                                         0...   C  ...15             0...   D  ...15
    0A      16xPI               4xGROUP|VERSION|TP|5xPTY|TA|MS|DI|2xCI(PS/DI POS)       8xAF|8xAF                   8xPSCHAR|8xPSCHAR
    0B      16xPI               4xGROUP|VERSION|TP|5xPTY|TA|MS|DI|2xCI(PS/DI POS)       16xPI                       8xPSCHAR|8xPSCHAR
    1A      16xPI               4xGROUP|VERSION|TP|5xPTY|5xPAGING                       LA|3xSLC|12xECC/TMC/LANG..  16xPIN
    1B      16xPI               4xGROUP|VERSION|TP|5xPTY                                16xPI                       16xPIN
    2A      16xPI               4xGROUP|VERSION|TP|5xPTY|RT AB|4xRT POS                 16xRT SEGMENT A             16xRT SEGMENT B
    2B      16xPI               4xGROUP|VERSION|TP|5xPTY|RT AB|4xRT POS                 16xPI                       16xRT
    3A      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          (ODA MSG)                   (AID)
    3B      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    4A      16xPI               4xGROUP|VERSION|TP|5xPTY|5xCT                           16xCT                       16xCT
    4B      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    5A      16xPI               4xGROUP|VERSION|TP|5xPTY|(TDC/ODA)                      (TDC/ODA)                   (TDC/ODA)
    5B      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    6A      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          (ODA)                       (ODA)
    6B      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    7A      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          (ODA)                       (ODA)
    7B      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    8A      16xPI               4xGROUP|VERSION|TP|5xPTY|(TMC/ODA)                      (TMC/ODA)                   (TMC/ODA)
    8B      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    9A      16xPI               4xGROUP|VERSION|TP|5xPTY|(EWS/ODA)                      (EWS/ODA)                   (EWS/ODA)
    9B      16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    10A     16xPI               4xGROUP|VERSION|TP|5xPTY|PTYN AB|4xPTYN POS             16xPTYN                     16xPTYN              
    10B     16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    11A     16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          (ODA)                       (ODA)
    11B     16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    12A     16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          (ODA)                       (ODA)
    12B     16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    13A     16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA/STY)                      (ODA/STY)                   (ODA/STY)
    13B     16xPI               4xGROUP|VERSION|TP|5xPTY|(ODA)                          16xPI                       (ODA)
    14A     16xPI               4xGROUP|VERSION|TP|5xPTY|EON TP|4xEON VARIANT           16xEON INFO                 16xEON PI
    14B     16xPI               4xGROUP|VERSION|TP|5xPTY|EON TP|EON TA                  16xPI                       16xEON PI
    15A     16xPI               4xGROUP|VERSION|TP|5xPTY|TA|(4x not used)|RBDS PS SEG.  16xRBDS PS                  16xRBDS PS
    15B     16xPI               4xGROUP|VERSION|TP|5xPTY||TA|MS|DI|2xCI(DI POS)         16xPI                       4xGROUP|VERSION|TP|5xPTY||TA|MS|DI|2xCI(DI POS)
*/

Rds::Rds() {}

void Rds::Reset() {
    Pi = {};
    Ecc = {};
    Ps = {};
    Rt = {};
}

void Rds::ReceiveData(RdsData data) {
    //Serial.println(data.BlockA,BIN);
    //Serial.println(data.BlockB,BIN);
    //Serial.println(data.BlockC,BIN);
    //Serial.println(data.BlockD,BIN);

    // Every A block contains PI
    ProcessPi(data.BlockA, data.ErrorA);
    // Every B block contains PTY
    ProcessPty(data.BlockB,data.ErrorB);

    //Serial.println(data.BlockB >> 12);
    //Detect version
    if (!(data.BlockB & 0b0000100000000000)) {
        // Verison A
        switch (data.BlockB >> 12) {
            case 0: ProcessPs(data.BlockB, data.BlockD, max(data.ErrorB, data.ErrorD)); ProcessAf(data.BlockC, max(data.ErrorC, data.ErrorB)); break;
            case 1: ProcessEcc(data.BlockC, max(data.ErrorC, data.ErrorB)); break;
            case 2: ProcessRtA(data.BlockB, data.BlockC, data.BlockD, std::max({data.ErrorB, data.ErrorC, data.ErrorD})); break;
            /* Unused RDS blocks
            case 3: break;
            case 4: break;
            case 5: break;
            case 6: break;
            case 7: break;
            case 8: break;
            case 9: break;
            case 10: break;
            case 11: break;
            case 12: break;
            case 13: break;
            case 14: break;
            case 15: break;
            */
        }
    }
    else {
        //Version B
        //Every B version's C block contains PI
        ProcessPi(data.BlockC, max(data.ErrorC, data.ErrorB));
        switch (data.BlockB >> 12) {
            case 0: ProcessPs(data.BlockB, data.BlockD, max(data.ErrorB, data.ErrorD)); break;
            //case 1: break;
            case 2: ProcessRtB(data.BlockB, data.BlockD, max(data.ErrorB, data.ErrorD)); break;
            /*case 3: break;
            case 4: break;
            case 5: break;
            case 6: break;
            case 7: break;
            case 8: break;
            case 9: break;
            case 10: break;
            case 11: break;
            case 12: break;
            case 13: break;
            case 14: break;
            case 15: break;*/
        }
    }
}

// TODO: Where detect PI mismatch??
/*
LOGIC
error == 0 => Store as realible PI

error > 0 => Push to buffer array
    Count same PI with error == 1 4 times => Realible PI
    Weighted average calculation in buffer array => Unrealible PI
*/
void Rds::ProcessPi(uint16_t blockAB_A_or_blockB_C, uint8_t errorAB_A_or_errorB_C) {
    // No error -> Store as stable like a boss
           // Serial.println(Pi.Value);

    if (errorAB_A_or_errorB_C == 0) {
        Pi.Value = blockAB_A_or_blockB_C;
        Pi.IsRealible = true;
        Pi.Available = true;
    } 
    // 1,2,3 error
    else {
        // weights: 8, 2, 1
        uint8_t weight = errorAB_A_or_errorB_C <= 1 ? 8 : 4 - errorAB_A_or_errorB_C;
        uint8_t lastSum = Pi.AverageSum;

        Pi.AverageSum = Safe8bitAdd(Pi.AverageSum, weight);

        // Calculate new average
        uint16_t temp = blockAB_A_or_blockB_C;
        for (size_t i = 0; i < 16; i++) {
            if (temp & 0x0001) {
                Pi.AverageRatios[15-i]=(Pi.AverageRatios[15-i]*lastSum+weight)/(lastSum+weight);
            } 
            else {
                 Pi.AverageRatios[15-i]=(Pi.AverageRatios[15-i]*lastSum-weight)/(lastSum+weight);
            }
            temp = temp >> 1;
        }

        // Collect at least 4 error==1 PIs or bunch of error>1 PIs AND (later) make sure every binary place's average is better than 0.1
        // -1.00 ... 0 -> probably 0
        // 0 ... 1.00  -> probably 1
        // TODO Define uncertain range 
        bool realible = Pi.AverageSum >= 32;
        uint16_t piTemp = 0;
        for (size_t i = 0; i < 16; i++)
        {
            //Serial.print(Pi.AverageRatios[15-i]);
            //Serial.print(" ");
            bool bit = Pi.AverageRatios[i] >= 0.0;
            if (abs(Pi.AverageRatios[i]) < 0.9) realible = false; 
            piTemp = piTemp << 1;
            piTemp = piTemp | bit;
        }

//Serial.print(": ");

        // Results
        Pi.Value = piTemp;
        Pi.IsRealible = realible;
        Pi.Available = true;


        //Serial.println(Pi.Value, HEX);

    }  
}

void Rds::ProcessEcc(uint16_t block1A_C, uint8_t error1A_C) {
    //if (!error1A_C) Serial.println(block1A_C & 0xFF,HEX);

    if (!error1A_C && !(block1A_C >> 11)) {
        Ecc.Value = block1A_C & 0xFF;
        Ecc.Available = true;

    }
}


/*
WEIGHTS:
0 -> 10
1 -> 8
2 -> 2
3 -> 1

Method 1: save ps chars to PS_CACHE_SIZE sized array:
if error == 0, 1 => save 
if error == 2, 3 => only save if "cell" is empty or saved error is 2, 3 
realible ps if => row has only 0 errors, 2 lines are the same

Method2:
*/
void Rds::ProcessPs(uint16_t block0AB_B, uint16_t block0AB_D, uint8_t error0AB_D) {
    // Raw data debug
   // Serial.println("**************************");
   /* Serial.print(block0AB_B & 0b11);
    Serial.print(" value: ");
    Serial.print(char(block0AB_D >> 8));
    Serial.print(char(block0AB_D & 0xFF));
    Serial.print(" bin: ");
    Serial.print(block0AB_D >> 8, BIN);
    Serial.print(block0AB_D & 0xFF, BIN);
    Serial.print(" error: ");
    Serial.print(error0AB_D);
    Serial.println();
  */
    /* INIT STUFF */
    bool foundRealible = false;
    // Char(s) position
    uint8_t select = block0AB_B & 0b11;
    // Weight of char(s)
    uint8_t weight = 0; 
    switch (error0AB_D) {
        case 0: weight = 10; break;
        case 1: weight = 8; break;
        case 2: weight = 2; break;
        case 3: weight = 1; break;
     }

    /* METHOD 1 STORE */
    // Calc method1 Cache row index and decrease error=0's weight
    if (select <= Ps.LastSelect) {
        Ps.CacheIndex = Ps.CacheIndex == PS_CACHE_SIZE - 1 ? 0 : Ps.CacheIndex + 1;
        for (size_t i = 0; i < 4; i++)
        {
            if (Ps.Cache[Ps.CacheIndex][i].Weight == 10) Ps.Cache[Ps.CacheIndex][i].Weight = 8;
        }
    }
    Ps.LastSelect = select;
    // STORE: store if current error is 0 OR 1 (>8) prev error is 2,3 OR invaled char was stored previously
    if (weight >= 8 || Ps.Cache[Ps.CacheIndex][select].Weight  < 8) {
        char low = block0AB_D >> 8;
        char high = block0AB_D & 0xFF;
        if (low >= 32) Ps.Cache[Ps.CacheIndex][select].Low = low;
        if (high >= 32) Ps.Cache[Ps.CacheIndex][select].High = high;
        Ps.Cache[Ps.CacheIndex][select].Weight = weight;
    }    

    /* METHOD 2 STORE */
    uint8_t lastSum = Ps.AverageSum[select];

    // New sum weight 
    Ps.AverageSum[select] = Ps.AverageSum[select] + weight > PS_MAX_WEIGHTSUM ? PS_MAX_WEIGHTSUM : Ps.AverageSum[select] + weight;

    // Calc averages
    uint16_t temp = block0AB_D;
    for (size_t i = 0; i < 16; i++) {
        if (temp & 1) {
            Ps.AverageRatios[select][15-i]=(Ps.AverageRatios[select][15-i]*lastSum+weight)/(lastSum+weight);
        } 
        else {
            Ps.AverageRatios[select][15-i]=(Ps.AverageRatios[select][15-i]*lastSum-weight)/(lastSum+weight);
        }
        temp = temp >> 1;
    }
    
    // Parse to char array
    char averagePs[8];
    for (size_t j = 0; j < 4; j++)
    {
        //char low;
        //char high;
        uint16_t psTemp = 0xFFFF;
        for (size_t i = 0; i < 16; i++)
        {
            //Serial.print(Ps.AverageRatios[j][i]);
            //Serial.print(" ");

            bool bit = Ps.AverageRatios[j][i] >= 0.0;
            //if (abs(Pi.AverageRatios[i]) < 0.9) realible = false; 
            psTemp = psTemp << 1;
            psTemp = psTemp | bit;

        }
        averagePs[2*j] = (psTemp>>8) >= 32 ? (psTemp>>8) : ' ' ;
        averagePs[2*j+1] = (psTemp&0xFF) >= 32 ? (psTemp&0xFF) : ' ';


/*        Serial.print(std::max({
            abs(Ps.AverageRatios[j][0]), 
            abs(Ps.AverageRatios[j][1]), 
            abs(Ps.AverageRatios[j][2]), 
            abs(Ps.AverageRatios[j][3]),
            abs(Ps.AverageRatios[j][4]), 
            abs(Ps.AverageRatios[j][5]), 
            abs(Ps.AverageRatios[j][6]), 
            abs(Ps.AverageRatios[j][7]),
            }));
        Serial.print(" ");
*/
    }

    /* VALIDATE RESULTS */
    /* METHOD 1 CHECK IF NOERROR LINE */
    bool complete = 
        Ps.Cache[Ps.CacheIndex][0].Weight && 
        Ps.Cache[Ps.CacheIndex][1].Weight && 
        Ps.Cache[Ps.CacheIndex][2].Weight && 
        Ps.Cache[Ps.CacheIndex][3].Weight;
    bool noError = 
        Ps.Cache[Ps.CacheIndex][0].Weight == 10 && 
        Ps.Cache[Ps.CacheIndex][1].Weight == 10 && 
        Ps.Cache[Ps.CacheIndex][2].Weight == 10 && 
        Ps.Cache[Ps.CacheIndex][3].Weight == 10;


    // No error line -> FOUND (result in Ps.Cache[Ps.CacheIndex])
    if (select == 3 && complete && noError) foundRealible = true;

    /* METHOD 1 SEARCH DUPLICATES */
    if (complete && !foundRealible) {
        /* Compare with METHOD2 average PS if its valid */
        bool equal = true;
        if (std::min({Ps.AverageSum[0], Ps.AverageSum[1], Ps.AverageSum[2], Ps.AverageSum[3]}) >= 16) {
            for (size_t i = 0; i < 4; i++)
            {
                if (Ps.Cache[Ps.CacheIndex][i].Low != averagePs[i*2] || Ps.Cache[Ps.CacheIndex][i].High != averagePs[i*2+1]) equal = false;
            }
            foundRealible = equal;
        }   

        if(!foundRealible) {
        /* Go through METHOD1 PS Cache  */
            for (uint8_t i = PS_CACHE_SIZE-1; i > 0; i--) {
                uint8_t j = (i + Ps.CacheIndex) % PS_CACHE_SIZE;

                equal = true;

                for (size_t k = 0; k < 4; k++)
                {
                    //Serial.print("origchar: ");
                    if (Ps.Cache[Ps.CacheIndex][k].High != Ps.Cache[j][k].High || Ps.Cache[Ps.CacheIndex][k].Low != Ps.Cache[j][k].Low) equal = false;
                }
                foundRealible = equal;
                if (equal) break;;
            }
        }
    }

    Ps.IsRealible = foundRealible;
    if (foundRealible) {
        for (size_t i = 0; i < 4; i++)
        {
            Ps.Value[i*2] = Ps.Cache[Ps.CacheIndex][i].Low;
            Ps.Value[i*2+1] = Ps.Cache[Ps.CacheIndex][i].High;
        }
    }
    else {
        for (size_t i = 0; i < 4; i++)
        {
            Ps.Value[i*2] = Ps.AverageSum[i] >= 8 ? averagePs[i*2] & 127 : ' '; 
            Ps.Value[i*2+1] = Ps.AverageSum[i] >= 8 ? averagePs[i*2+1] & 127 : ' '; 
        }
    }
    Ps.Value[8] = '\0';
    Ps.Available = true;

/*

    Serial.print(" (sums: ");
    Serial.print(Ps.AverageSum[0]);
    Serial.print(" ");
    Serial.print(Ps.AverageSum[1]);
    Serial.print(" ");
    Serial.print(Ps.AverageSum[2]);
    Serial.print(" ");
    Serial.print(Ps.AverageSum[3]);
    Serial.print(") ");*/
    //Serial.println();
}

void Rds::ProcessPty(uint16_t blockAB_B, uint8_t errorAB_B) {

}

void Rds::ProcessRtA(uint16_t block2A_B, uint16_t block2A_C, uint16_t block2A_D, uint8_t error2A_BCD) {
    if (error2A_BCD <= 1) {
        if (bool(block2A_B & 16) != Rt.IsA) {
            // AB flag changed
            Rt.IsA = !Rt.IsA;
            std::fill(Rt.Ready, Rt.Ready+16, false);
            
        }

        //Store 2*2 char
        uint8_t pos = (block2A_B & 15);
        Rt.Value[pos*4] = (block2A_C>>8) >= 32 ? (block2A_C>>8) : ' ';
        Rt.Value[pos*4+1] = (block2A_C & 0xFF) >= 32 ? (block2A_C & 0xFF) : ' ';
        Rt.Value[pos*4+2] = (block2A_D>>8) >= 32 ? (block2A_D>>8) : ' ';
        Rt.Value[pos*4+3] = (block2A_D & 0xFF) >= 32 ? (block2A_D & 0xFF) : ' ';

        //Serial.print(" pos: ");   
        Rt.Ready[pos] = true;

        bool complete = true;
        for (size_t i = 0; i < 16; i++)
        {
            /*Serial.print(Rt.Ready[i]);
            Serial.print(" ");*/
            if (!Rt.Ready[i]) complete = false;
        }
        if (complete) { 
            /*Serial.print(" result: ");
            Serial.println(Rt.Value);*/
            Rt.Available = true;
        }
        /*Serial.print(char(block2A_C>>8));
        Serial.print(char(block2A_C&0xFF));
        Serial.print(char(block2A_D>>8));
        Serial.print(char(block2A_D&0xFF));*/
        //Serial.println();

    }
}

void Rds::ProcessRtB(uint16_t block2B_B, uint16_t block2B_D, uint8_t error2B_BD) {

}

void Rds::ProcessAf(uint16_t block0A_C, uint8_t error0A_C) {

}

bool Rds::ReadPi(uint16_t *result, bool *realible) {
    if (Pi.Available) {
        *result = Pi.Value;
        *realible = Pi.IsRealible;
        Pi.Available = false;
        return true;
    }
    return false;
}

bool Rds::ReadEcc(uint8_t *result) {
    if (Ecc.Available) {
        *result = Ecc.Value;
        Ecc.Available = false;
        return true;
    }
    return false;
}

bool Rds::ReadPs(char *result, bool *realible) {
     if (Ps.Available) {
        strcpy(result, Ps.Value);
        *realible = Ps.IsRealible;
        Pi.Available = false;
        return true;
    }
    return false;  
}

bool Rds::ReadRt(char *result) {
    if (Rt.Available) {
        strcpy(result, Rt.Value);
        Rt.Available = false;
        return true;
    }
    return false;
}

uint8_t Rds::Safe8bitAdd(uint8_t num, uint8_t add) {
    return uint8_t(num + add) < num ? UINT8_MAX : num + add; 
}

