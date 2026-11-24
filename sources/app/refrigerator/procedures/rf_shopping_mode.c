#include "rf_shopping_mode.h"


void rfShoppingModeInit(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (rfP->Sets.user.functions.Shopping)
        rfP->Values.shoppingModeTimer = rfC->Parameters.shoppingModeTime;
}

void rfShoppingMode1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);
    if (rfP->Values.shoppingModeTimer)
        rfP->Values.shoppingModeTimer--;
}
