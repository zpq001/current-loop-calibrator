

#include <QApplication>
#include "linear_calibration.h"





static calibration_t adc_calibration;

volatile uint32_t temp32u;
volatile int32_t temp32;


void check32(int32_t val, int32_t ref) {
    if (val != ref)
         qDebug("Value %d not equal to reference %d",val, ref);
}


void TDD_VerifyCalibration(void) {

    temp32 = DivI32Rnd(1550,100);
    check32(temp32, 15);

    temp32 = DivI32Rnd(1549,100);
    check32(temp32, 15);

    temp32 = DivI32Rnd(1551,100);
    check32(temp32, 16);

    temp32 = DivI32Rnd(1550,-100);
    check32(temp32, -15);

    temp32 = DivI32Rnd(1549,-100);
    check32(temp32, -15);

    temp32 = DivI32Rnd(1551,-100);
    check32(temp32, -16);

    temp32 = DivI32Rnd(-1550,100);
    check32(temp32, -15);

    temp32 = DivI32Rnd(-1549,100);
    check32(temp32, -15);

    temp32 = DivI32Rnd(-1551,100);
    check32(temp32, -16);

    temp32 = DivI32Rnd(-1550,-100);
    check32(temp32, 15);

    temp32 = DivI32Rnd(-1549,-100);
    check32(temp32, 15);

    temp32 = DivI32Rnd(-1551,-100);
    check32(temp32, 16);




    adc_calibration.point1.value = 0;
    adc_calibration.point1.code = 2000;
    adc_calibration.point2.value = 40000;
    adc_calibration.point2.code = 4000;
    adc_calibration.scale = 1;
    CalculateCoefficients(&adc_calibration);

    // First basic functions at calibration points
    temp32u = adc_calibration.point1.code;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, adc_calibration.point1.value);

    temp32u = adc_calibration.point2.code;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, adc_calibration.point2.value);

    temp32u = 2000 + 100;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, 2000);

    temp32u = 2000 - 100;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, -2000);

    temp32u = 2000 + 113;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, 2260);

    temp32u = 2000 - 113;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, -2260);

    //-----------------------------------------------------//


    adc_calibration.point1.value = 0;
    adc_calibration.point1.code = 2000;
    adc_calibration.point2.value = 400000;
    adc_calibration.point2.code = 4000;
    adc_calibration.scale = 1;
    CalculateCoefficients(&adc_calibration);

    // First basic functions at calibration points
    temp32u = adc_calibration.point1.code;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, adc_calibration.point1.value);

    temp32u = adc_calibration.point2.code;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, adc_calibration.point2.value);

    temp32u = 2000 + 100;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, 20000);

    temp32u = 2000 - 100;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, -20000);

    temp32u = 2000 + 113;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, 22600);

    temp32u = 2000 - 113;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, -22600);



    //-----------------------------------------------------//


    adc_calibration.point1.value = 0;
    adc_calibration.point1.code = 2047;
    adc_calibration.point2.value = 400000;
    adc_calibration.point2.code = 3859;
    adc_calibration.scale = 1000;
    CalculateCoefficients(&adc_calibration);

    // First basic functions at calibration points
    temp32u = adc_calibration.point1.code;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, adc_calibration.point1.value);

    temp32u = adc_calibration.point2.code;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, adc_calibration.point2.value);

    temp32u = 2047 + 100;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, 22075);

    temp32u = 2047 - 100;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, -22075);

    temp32u = 2047 + 113;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, 24945);

    temp32u = 2047 - 113;
    temp32 = GetValueForCode(&adc_calibration, temp32u);
    check32(temp32, -24945);
}


#define WF_BUFFER_SIZE  2000
uint16_t buffer[WF_BUFFER_SIZE];

void TDD_VerifyWaveformGenerator(void) {
    CreateSawWaveform(buffer, 3987, 2654, WF_BUFFER_SIZE);
}




