//#include "CabinALL_Performance.h"
//#include "project.h"
/*
void CabinPerformance1MinTask( stCabin_Ram *cabin_RPtr ){   
  if( cabin_RPtr->values.performance.timer < 0xFF )
    cabin_RPtr->values.performance.timer++;
}

void CabinPerformanceInit( stCabin_Ram *cabin_RPtr ){
  cabin_RPtr->values.performance.checked = false;
  cabin_RPtr->values.performance.OK = false;    
  cabin_RPtr->values.performance.timer = 0;
  cabin_RPtr->values.performance.airTempAtStart = 
      cabin_RPtr->components.airTempSensor.valueInC; 
}

void CabinPerformanceCheck( stCabin_Flash const* cabin_FPtr,
                            stCabin_Ram *cabin_RPtr ){  
  signed char deltaTempAir;
  
  if( cabin_RPtr->values.performance.checked ) return;
  
  if( cabin_FPtr->components.evaTempSensor.type ){
    if( ( cabin_RPtr->values.performance.timer > cabin_FPtr->parameters.Performance.timeEva1 ) &&
        ( cabin_RPtr->values.performance.timer < cabin_FPtr->parameters.Performance.timeEva2 ) ){
      if( ( cabin_RPtr->components.evaTempSensor.value > cabin_FPtr->parameters.Performance.tempEva1 ) ||
          ( cabin_RPtr->components.evaTempSensor.value < cabin_FPtr->parameters.Performance.tempEva2 ) ){
        cabin_RPtr->values.performance.checked = true;
      }        
    }
  }
          
  if( cabin_FPtr->components.airTempSensor.type ){
    if( ( cabin_RPtr->values.performance.timer > cabin_FPtr->parameters.Performance.timeAir ) ){ 
      deltaTempAir =    
        ( cabin_RPtr->values.performance.airTempAtStart -
          cabin_RPtr->components.airTempSensor.valueInC );  
      if( deltaTempAir < cabin_FPtr->parameters.Performance.deltaTempAir ){ 
        cabin_RPtr->values.performance.checked = true;
      }
    }
  }
          
  if( ( cabin_RPtr->values.performance.timer > cabin_FPtr->parameters.Performance.timeEva1 ) && 
      ( cabin_RPtr->values.performance.timer > cabin_FPtr->parameters.Performance.timeEva2 ) &&
      ( cabin_RPtr->values.performance.timer > cabin_FPtr->parameters.Performance.timeAir  ) ){
    cabin_RPtr->values.performance.OK = true;  
    cabin_RPtr->values.performance.checked = true;
  }  
}


*/
