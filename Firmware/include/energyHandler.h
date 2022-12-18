#include "EmonLib.h" // Include Emon Library
EnergyMonitor emon1; // Create an instance
EnergyMonitor emon2; // Create an instance

String energyData = "";

double realPower1 = 0.0;
double apparentPower1 = 0.0;
double powerFActor1 = 0.0;
double supplyVoltage1 = 0.0;
double Irms1 = 0.0;

double Irms2 = 0.0;
double apparentPower2 = 0.0;
double realPower2 = 0.0;
double powerFActor2 = 0.0;
double supplyVoltage2 = 0.0;

void setupEnergyHander()
{
    emon1.voltage(VP, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
    emon2.voltage(VP, vCalibration, 1.7);
    emon1.current(IO34, currCalibration_A);     // input pin, calibration
    emon2.current(VN, currCalibration_B);       // input pin, calibration
}

void loopEmon()
{
    emon1.calcVI(20, 2000); // Calculate all. No.of half wavelengths (crossings), time-out

    emon1.serialprint(); // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)

    realPower1 = emon1.realPower;         // extract Real Power into variable
    apparentPower1 = emon1.apparentPower; // extract Apparent Power into variable
    powerFActor1 = emon1.powerFactor;     // extract Power Factor into Variable
    supplyVoltage1 = emon1.Vrms;          // extract Vrms into Variable
    Irms1 = emon1.Irms;                   // extract Irms into Variable

    Irms2 = emon2.calcIrms(1480); // Calculate Irms only
    apparentPower2 = Irms2 * 230;
    realPower2 = emon2.realPower;
    powerFActor2 = emon2.powerFactor; // extract Power Factor into Variable
    supplyVoltage2 = emon1.Vrms;  
    energyData = String(realPower1) + String(";") + String(apparentPower1) + String(";") +
                 String(powerFActor1) + String(";") + String(supplyVoltage1) + String(";") + String(Irms1) +
                 String(";") + String(Irms2) + String(";") + String(apparentPower2);
}

String getEnergyData()
{
    return energyData;
}