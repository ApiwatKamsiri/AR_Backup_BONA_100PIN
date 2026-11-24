#include "cabin_sets_proc.h"


void cabinUserSets(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

    unsigned char userIndex;

    if (cabinP->values.errSensors)
    {
        cabinP->sets.user.functions.Vacation = false;
        cabinP->sets.user.functions.Rice = false;
        cabinP->E2.Vacation = false;
        cabinP->E2.Rice = false;
    }

    switch (cabinP->sets.user.Mode)
    {
        case _CabinMode_FF:
            if (cabinP->sets.user.functions.Vacation)
                userIndex = cabinC->sets.indexes.functions.Vacation;
            else if (cabinP->sets.user.functions.Quick)
                userIndex = cabinC->sets.indexes.functions.QuickFF;
            else if (cabinP->sets.user.functions.Rice)
                userIndex = cabinC->sets.indexes.functions.Rice;
            else
                userIndex = cabinC->sets.indexes.FF[cabinP->sets.user.FF];
        break;

        case _CabinMode_FRZ:

            /*
            if( cabinP->sets.work.functions.EcoExtra ){
            userIndex = cabinC->sets.indexes.FRZ[cabinP->sets.user.FRZ];
            }else{*/
            if (cabinP->sets.user.functions.Quick)
                userIndex = cabinC->sets.indexes.functions.QuickFRZ;
            else if( cabinP->sets.user.functions.Convertible )
              userIndex = cabinC->sets.indexes.functions.Convertible[cabinP->sets.user.Convertible];
            else
                userIndex = cabinC->sets.indexes.FRZ[cabinP->sets.user.FRZ];
            /*} */
        break;

        default:
            userIndex = 0;
        break;
    }

    cabinP->sets.datas.display = cabinSetsDatasTable_F[userIndex].display;
}

void cabinUserSetsCompSpeedUpInit(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
  
    unsigned char MaxDial;
    
    switch (cabinP->sets.user.Mode)
    {
        case _CabinMode_FF:
            MaxDial = cabinSetsDatasTable_F[cabinC->sets.indexes.FF[0]].display;
            
            for(char i=0; i<sizeof(cabinC->sets.indexes.FF); i++)
            {
              if(cabinSetsDatasTable_F[cabinC->sets.indexes.FF[i]].display < MaxDial)
                MaxDial = cabinSetsDatasTable_F[cabinC->sets.indexes.FF[i]].display;
              else
                MaxDial = MaxDial;
            }
          break;
          
        case _CabinMode_FRZ:
            MaxDial = cabinSetsDatasTable_F[cabinC->sets.indexes.FRZ[0]].display;
            
            for(char i=0; i<sizeof(cabinC->sets.indexes.FRZ); i++)
            {
              if(cabinSetsDatasTable_F[cabinC->sets.indexes.FRZ[i]].display < MaxDial)
                MaxDial = cabinSetsDatasTable_F[cabinC->sets.indexes.FRZ[i]].display;
              else
                MaxDial = MaxDial;
            }
          break;
        default:
            MaxDial = 0;
          break;
    }  
    cabinP->sets.user.MaxDialUserSet = MaxDial;
}