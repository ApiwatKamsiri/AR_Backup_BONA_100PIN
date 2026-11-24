/*
#include "RefrigeratorVacation.h"
#include "E2Config.h"
*/
#include "rf_vacation.h"

void rfVacationTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
  bool Vacation = false;

  for (unsigned char i = 0; i < _NumOfCabin; i++)
  {
    if (rfC->Cabin[i].type != _CabinType_NONE)
    {
      if (rfP->Cabin[i].sets.user.functions.Vacation)
        Vacation = true;
    }
  }

  rfP->Sets.user.functions.Vacation = Vacation;
}

void rfVacationEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
  UNUSED(rfC);

  rfP->Sets.user.functions.Vacation = true;
}

void rfVacationDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
  UNUSED(rfC);
  
  rfP->Sets.user.functions.Vacation = false;
}

void rfVacationToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
  UNUSED(rfC);
  
  rfP->Sets.user.functions.Vacation = !rfP->Sets.user.functions.Vacation;
}
