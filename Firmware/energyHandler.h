#include "EmonLib.h" // Include Emon Library
EnergyMonitor emon1; // Create an instance
EnergyMonitor emon2; // Create an instance

String energyData = "";
void setupEnergyHander()
{
    emon1.voltage(VP, 234.26, 1.7); // Voltage: input pin, calibration, phase_shift
    emon1.current(IO34, 111.1);     // input pin, calibration
    emon2.current(VN, 111.1);       // input pin, calibration
}

void loopEmon()
{
    emon1.calcVI(20, 2000); // Calculate all. No.of half wavelengths (crossings), time-out

    emon1.serialprint(); // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)

    float realPower = emon1.realPower;         // extract Real Power into variable
    float apparentPower1 = emon1.apparentPower; // extract Apparent Power into variable
    float powerFActor = emon1.powerFactor;     // extract Power Factor into Variable
    float supplyVoltage = emon1.Vrms;          // extract Vrms into Variable
    float Irms1 = emon1.Irms;                   // extract Irms into Variable

    double Irms2 = emon2.calcIrms(1480); // Calculate Irms only
    double apparentPower2 = Irms2 * 230;

    energyData=String(realPower)+String(";")+String(apparentPower1)+String(";")+
    String(powerFActor)+String(";")+String(supplyVoltage)+String(";")+String(Irms1)+
    String(";")+String(Irms2)+String(";")+String(apparentPower2);
}

String getEnergyData(){
    return energyData;
}