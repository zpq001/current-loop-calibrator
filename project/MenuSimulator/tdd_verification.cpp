

#include <QApplication>
#include "linear_calibration.h"
#include "utils.h"




static calibration_t adc_calibration;

uint32_t temp32u;
int32_t temp32;


void check32(int32_t val, int32_t ref) {
    if (val != ref)
         qDebug("Value %d not equal to reference %d",val, ref);
}

void check32u(uint32_t val, uint32_t ref) {
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


void TDD_VerifyRound(void) {

    temp32u = 15349;
    round_uint32(&temp32u, 0);
    check32u(temp32u, 15350);

    temp32u = 15346;
    round_uint32(&temp32u, 0);
    check32u(temp32u, 15350);

    temp32u = 15345;
    round_uint32(&temp32u, 0);
    check32u(temp32u, 15350);

    temp32u = 15344;
    round_uint32(&temp32u, 0);
    check32u(temp32u, 15340);

    temp32u = 15341;
    round_uint32(&temp32u, 0);
    check32u(temp32u, 15340);

    temp32u = 15340;
    round_uint32(&temp32u, 0);
    check32u(temp32u, 15340);


    temp32u = 15349;
    round_uint32(&temp32u, 1);
    check32u(temp32u, 15300);

    temp32u = 15350;
    round_uint32(&temp32u, 1);
    check32u(temp32u, 15400);

    temp32u = 15351;
    round_uint32(&temp32u, 1);
    check32u(temp32u, 15400);

    temp32u = 00001;
    round_uint32(&temp32u, 1);
    check32u(temp32u, 00000);


    temp32u = 15501;
    round_uint32(&temp32u, 2);
    check32u(temp32u, 16000);

    temp32u = 15499;
    round_uint32(&temp32u, 2);
    check32u(temp32u, 15000);

    temp32u = 15999;
    round_uint32(&temp32u, 2);
    check32u(temp32u, 16000);

    temp32u = 15123;
    round_uint32(&temp32u, 2);
    check32u(temp32u, 15000);

    temp32u = 00001;
    round_uint32(&temp32u, 2);
    check32u(temp32u, 00000);


}



void TDD_VerifyRoundSigned(void) {

    temp32 = 15349;
    round_int32(&temp32, 0);
    check32(temp32, 15350);

    temp32 = 15346;
    round_int32(&temp32, 0);
    check32(temp32, 15350);

    temp32 = 15345;
    round_int32(&temp32, 0);
    check32(temp32, 15350);

    temp32 = 15344;
    round_int32(&temp32, 0);
    check32(temp32, 15340);

    temp32 = 15341;
    round_int32(&temp32, 0);
    check32(temp32, 15340);

    temp32 = 15340;
    round_int32(&temp32, 0);
    check32(temp32, 15340);


    temp32 = 15349;
    round_int32(&temp32, 1);
    check32(temp32, 15300);

    temp32 = 15350;
    round_int32(&temp32, 1);
    check32(temp32, 15400);

    temp32 = 15351;
    round_int32(&temp32, 1);
    check32(temp32, 15400);

    temp32 = 00001;
    round_int32(&temp32, 1);
    check32(temp32, 00000);


    temp32 = 15501;
    round_int32(&temp32, 2);
    check32(temp32, 16000);

    temp32 = 15499;
    round_int32(&temp32, 2);
    check32(temp32, 15000);

    temp32 = 15999;
    round_int32(&temp32, 2);
    check32(temp32, 16000);

    temp32 = 15123;
    round_int32(&temp32, 2);
    check32(temp32, 15000);

    temp32 = 00001;
    round_int32(&temp32, 2);
    check32(temp32, 00000);

    //------------------------------------//

    temp32 = -15349;
    round_int32(&temp32, 0);
    check32(temp32, -15350);

    temp32 = -15346;
    round_int32(&temp32, 0);
    check32(temp32, -15350);

    temp32 = -15345;
    round_int32(&temp32, 0);
    check32(temp32, -15350);

    temp32 = -15344;
    round_int32(&temp32, 0);
    check32(temp32, -15340);

    temp32 = -15341;
    round_int32(&temp32, 0);
    check32(temp32, -15340);

    temp32 = -15340;
    round_int32(&temp32, 0);
    check32(temp32, -15340);


    temp32 = -15349;
    round_int32(&temp32, 1);
    check32(temp32, -15300);

    temp32 = -15350;
    round_int32(&temp32, 1);
    check32(temp32, -15400);

    temp32 = -15351;
    round_int32(&temp32, 1);
    check32(temp32, -15400);

    temp32 = -00001;
    round_int32(&temp32, 1);
    check32(temp32, 0);


    temp32 = -15501;
    round_int32(&temp32, 2);
    check32(temp32, -16000);

    temp32 = -15499;
    round_int32(&temp32, 2);
    check32(temp32, -15000);

    temp32 = -15999;
    round_int32(&temp32, 2);
    check32(temp32, -16000);

    temp32 = -15123;
    round_int32(&temp32, 2);
    check32(temp32, -15000);

    temp32 = -00001;
    round_int32(&temp32, 2);
    check32(temp32, 0);


}
