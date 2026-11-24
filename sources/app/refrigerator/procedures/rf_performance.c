#include "rf_performance.h"
/*
void rfPerformanceInit(){  
  for( unsigned char i = 0; i < _NumOfCabin; i++ ){  
    CabinPerformanceInit( &refrigerator_F.Cabin[i], &refrigerator_R.Cabin[i] );
  }
}

void rfPerformanceTask(){
  IceCreamMachineSets( &refrigerator_R.IceCreamMachine );
  IceCreamMachineTask( &refrigerator_F.IceCreamMachine, &refrigerator_R.IceCreamMachine);
  
  IceMachineTask( &refrigerator_F.IceMachine, &refrigerator_R.IceMachine);
  DispenserTask( &refrigerator_F.Dispenser, &refrigerator_R.Dispenser);
  refrigerator_R.WaterSource.values.dispenserDesired = refrigerator_R.Dispenser.values.waterDesired;
  refrigerator_R.WaterSource.values.iceMachineDesired = refrigerator_R.IceMachine.values.waterDesired;
  refrigerator_R.IceSource.values.dispenserDesired = refrigerator_R.Dispenser.values.iceDesired; 
  
  WaterSourceTask( &refrigerator_F.WaterSource, &refrigerator_R.WaterSource);  
  refrigerator_R.Dispenser.values.waterApplied = refrigerator_R.WaterSource.values.dispenserApplied;
  refrigerator_R.IceMachine.values.waterApplied = refrigerator_R.WaterSource.values.iceMachineApplied; 
  
  IceSourceTask( &refrigerator_F.IceSource, &refrigerator_R.IceSource);
  refrigerator_R.Dispenser.values.iceApplied = refrigerator_R.IceSource.values.dispenserApplied;  
  
   
  for( unsigned char i = 0; i < _NumOfCabin; i++ ){
    CabinUserSets( &refrigerator_F.Cabin[i], &refrigerator_R.Cabin[i] );
    
    if( !UI_SetChangeTimer )
      CabinWorkSets( &refrigerator_F.Cabin[i], &refrigerator_R.Cabin[i] );
    
    CabinTask( &refrigerator_F.Cabin[i], &refrigerator_R.Cabin[i] );
    
    if( refrigerator_F.Cabin[i].type != _CabinType_NONE ){
      refrigerator_R.Cooler.values.gas[i].desired = 
        refrigerator_R.Cabin[i].values.gas.desired;     
    
      refrigerator_R.Cabin[i].values.gas.applied = 
        refrigerator_R.Cooler.values.gas[i].applied;    
    }else{
      refrigerator_R.Cooler.values.gas[i].desired = _GasValue_0;
    }
  } 
  
  CabinsQuickFRZ( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin );
  CabinsQuickFF( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin );
  CabinsAirSupply( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin );
  CabinsFRZDefrost( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin );
  CabinsQuickIceCream( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin,
                       &refrigerator_F.IceCreamMachine, &refrigerator_R.IceCreamMachine );
  CoolerTask( &refrigerator_F.Cooler, &refrigerator_R.Cooler );   
  
  RefrigeratorEconomyTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorVacationTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorRiceTask( &refrigerator_F, &refrigerator_R );
  rfSabbathTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorEcoExtraTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorDoorsClosedTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorDoorsAlertTask( &refrigerator_F, &refrigerator_R );
}


void rfPerformanceTask(){
  IceCreamMachineSets( &refrigerator_R.IceCreamMachine );
  IceCreamMachineTask( &refrigerator_F.IceCreamMachine, &refrigerator_R.IceCreamMachine);
  
  IceMachineTask( &refrigerator_F.IceMachine, &refrigerator_R.IceMachine);
  DispenserTask( &refrigerator_F.Dispenser, &refrigerator_R.Dispenser);
  refrigerator_R.WaterSource.values.dispenserDesired = refrigerator_R.Dispenser.values.waterDesired;
  refrigerator_R.WaterSource.values.iceMachineDesired = refrigerator_R.IceMachine.values.waterDesired;
  refrigerator_R.IceSource.values.dispenserDesired = refrigerator_R.Dispenser.values.iceDesired; 
  
  WaterSourceTask( &refrigerator_F.WaterSource, &refrigerator_R.WaterSource);  
  refrigerator_R.Dispenser.values.waterApplied = refrigerator_R.WaterSource.values.dispenserApplied;
  refrigerator_R.IceMachine.values.waterApplied = refrigerator_R.WaterSource.values.iceMachineApplied; 
  
  IceSourceTask( &refrigerator_F.IceSource, &refrigerator_R.IceSource);
  refrigerator_R.Dispenser.values.iceApplied = refrigerator_R.IceSource.values.dispenserApplied;  
  
   
  for( unsigned char i = 0; i < _NumOfCabin; i++ ){ 
    CabinUserSets( &refrigerator_F.Cabin[i], &refrigerator_R.Cabin[i] );
    
    if( !UI_SetChangeTimer )
      CabinWorkSets( &refrigerator_F.Cabin[i], &refrigerator_R.Cabin[i] );
    
    CabinPerformanceTask( &refrigerator_F.Cabin[i], &refrigerator_R.Cabin[i] );
    
    if( refrigerator_F.Cabin[i].type != _CabinType_NONE ){
      refrigerator_R.Cooler.values.gas[i].desired = 
        refrigerator_R.Cabin[i].values.gas.desired;     
    
      refrigerator_R.Cabin[i].values.gas.applied = 
        refrigerator_R.Cooler.values.gas[i].applied;    
    }else{
      refrigerator_R.Cooler.values.gas[i].desired = _GasValue_0;
    }
  } 
  
  CabinsQuickFRZ( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin );
  CabinsQuickFF( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin );
  CabinsAirSupply( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin );
  CabinsFRZDefrost( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin );
  CabinsQuickIceCream( refrigerator_F.Cabin, refrigerator_R.Cabin, _NumOfCabin,
                       &refrigerator_F.IceCreamMachine, &refrigerator_R.IceCreamMachine );
  CoolerTask( &refrigerator_F.Cooler, &refrigerator_R.Cooler );   
  
  RefrigeratorEconomyTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorVacationTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorRiceTask( &refrigerator_F, &refrigerator_R );
  rfSabbathTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorEcoExtraTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorDoorsClosedTask( &refrigerator_F, &refrigerator_R );
  RefrigeratorDoorsAlertTask( &refrigerator_F, &refrigerator_R );
}

void rfDriver(){
  for( unsigned char i = 0; i < _NumOfCabin; i++ ){ 
    CabinDriver( &refrigerator_F.Cabin[i], &refrigerator_R.Cabin[i] );
  }
  
  CoolerDriver( &refrigerator_F.Cooler, &refrigerator_R.Cooler );
  IceMachineDriver( &refrigerator_F.IceMachine, &refrigerator_R.IceMachine);
  DispenserDriver( &refrigerator_F.Dispenser, &refrigerator_R.Dispenser);
  WaterSourceDriver( &refrigerator_F.WaterSource, &refrigerator_R.WaterSource);
  IceSourceDriver( &refrigerator_F.IceSource, &refrigerator_R.IceSource);
  IceCreamMachineDriver( &refrigerator_F.IceCreamMachine, &refrigerator_R.IceCreamMachine);  
}
*/
