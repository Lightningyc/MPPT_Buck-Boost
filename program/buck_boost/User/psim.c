//
//
//#include <Stdlib.h>
//#include <String.h>
//#include <math.h>
//#include <Psim.h>
//// PLACE GLOBAL VARIABLES OR USER FUNCTIONS HERE...
//#define  Interruption_frequency 50000
//#define  voltage_coefficient (21.0f/4095)
//#define Current_coefficient (3.0f/3276)
//#define power_coefficient  (voltage_coefficient*Current_coefficient)
//
//unsigned int run_time = 0;
//unsigned int Vin_adc = 0;
//unsigned int Iin_adc = 0;
//unsigned int Vo_adc = 0;
//unsigned int Io_adc = 0;
//float D = 0.0f;
//
//#define P (0.0005f)
//#define I (0.000001f)
//
//typedef struct {
//    int mppt_vin_target;
//    int mppt_deltaV;
//    unsigned int power;
//    unsigned int last_power;
//    unsigned int vin;
//    unsigned int vin_last;
//    int mppt_vin_error;
//    int mppt_vin_error_integral;
//    float mppt_contral_output;
//} mppt_struction;
//mppt_struction mttp = {(int) (20.0f / voltage_coefficient), (int) (0.01f / voltage_coefficient), 0, 0,
//                       0, 0, 0, 0, 0};
//
//
//void mppt_Perturb_and_Observe(mppt_struction *mppt, const unsigned int *Vo_ADC_value, const unsigned int *I_ADC_value,
//                              const unsigned int *vin_ADC_value) {
//
//    static unsigned int mppt_count = 0;
//    mppt_count++;
//    mppt->power = *vin_ADC_value * *I_ADC_value;
//    mppt->vin = *vin_ADC_value;
//    if (mppt_count == 100) {
//        mppt_count = 0;
//        if (mppt->power > mppt->last_power) {
//            if (mppt->vin > mppt->vin_last) {
//                mppt->mppt_vin_target = mppt->mppt_vin_target + mppt->mppt_deltaV;
//            } else if (mppt->vin < mppt->vin_last) {
//                mppt->mppt_vin_target = mppt->mppt_vin_target - mppt->mppt_deltaV;
//            }
//        } else if (mppt->power < mppt->last_power) {
//            if (mppt->vin < mppt->vin_last) {
//                mppt->mppt_vin_target = mppt->mppt_vin_target + mppt->mppt_deltaV;
//            } else if (mppt->vin > mppt->vin_last) {
//                mppt->mppt_vin_target = mppt->mppt_vin_target - mppt->mppt_deltaV;
//            }
//        }
//        mppt->last_power = mppt->power;
//        mppt->vin_last = mppt->vin;
//
//    };
//
//
//    mppt->mppt_vin_error = (int) *vin_ADC_value - mppt->mppt_vin_target;
//    mppt->mppt_contral_output = P * (float) mppt->mppt_vin_error + I * (float) mppt->mppt_vin_error_integral;
//    mppt->mppt_vin_error_integral = mppt->mppt_vin_error_integral + mppt->mppt_vin_error;
//
//
//    if (mppt->mppt_contral_output > 0.9f) {
//        mppt->mppt_contral_output = 0.9f;
//    }
//    if (mppt->mppt_contral_output < 0.0f) {
//        mppt->mppt_contral_output = 0.0f;
//    }
//
//
//}
//
///////////////////////////////////////////////////////////////////////
//// FUNCTION: SimulationStep
////   This function runs at every time step.
////double t: (read only) time
////double delt: (read only) time step as in Simulation control
////double *in: (read only) zero based array of input values. in[0] is the first node, in[1] second input...
////double *out: (write only) zero based array of output values. out[0] is the first node, out[1] second output...
////int *pnError: (write only)  assign  *pnError = 1;  if there is an error and set the error message in szErrorMsg
////    strcpy(szErrorMsg, "Error message here...");
//// DO NOT CHANGE THE NAME OR PARAMETERS OF THIS FUNCTION
//void SimulationStep(
//        double t, double delt, double *in, double *out,
//        int *pnError, char *szErrorMsg,
//        void **reserved_UserData, int reserved_ThreadIndex, void *reserved_AppPtr) {
//// ENTER YOUR CODE HERE...
//    unsigned int begin_code_time = (unsigned int) (1 / (Interruption_frequency * delt));
//
//    if (run_time == begin_code_time) {
//        run_time = 0;
//        Vin_adc = (unsigned int) in[0];
//        Iin_adc = (unsigned int) in[1];
//        Vo_adc = (unsigned int) in[2];
//        Io_adc = (unsigned int) in[3];
//        mppt_Perturb_and_Observe(&mttp, &Vo_adc, &Iin_adc, &Vin_adc);
//
//
//    } else {
//        run_time++;
//    }
//
//    out[0] = mttp.mppt_vin_error;
//    out[1] = mttp.power - mttp.last_power;
//    out[2] = mttp.mppt_vin_error_integral;
//    out[3] = mttp.vin;
//    out[4] = mttp.mppt_contral_output;
//}
//
//
///////////////////////////////////////////////////////////////////////
//// FUNCTION: SimulationBegin
////   Initialization function. This function runs once at the beginning of simulation
////   For parameter sweep or AC sweep simulation, this function runs at the beginning of each simulation cycle.
////   Use this function to initialize static or global variables.
////const char *szId: (read only) Name of the C-block
////int nInputCount: (read only) Number of input nodes
////int nOutputCount: (read only) Number of output nodes
////int nParameterCount: (read only) Number of parameters is always zero for C-Blocks.  Ignore nParameterCount and pszParameters
////int *pnError: (write only)  assign  *pnError = 1;  if there is an error and set the error message in szErrorMsg
////    strcpy(szErrorMsg, "Error message here...");
//// DO NOT CHANGE THE NAME OR PARAMETERS OF THIS FUNCTION
//void SimulationBegin(
//        const char *szId, int nInputCount, int nOutputCount,
//        int nParameterCount, const char **pszParameters,
//        int *pnError, char *szErrorMsg,
//        void **reserved_UserData, int reserved_ThreadIndex, void *reserved_AppPtr) {
//// ENTER INITIALIZATION CODE HERE...
//
//
//
//}
//
//
///////////////////////////////////////////////////////////////////////
//// FUNCTION: SimulationEnd
////   Termination function. This function runs once at the end of simulation
////   For parameter sweep or AC sweep simulation, this function runs at the end of each simulation cycle.
////   Use this function to de-allocate any allocated memory or to save the result of simulation in an alternate file.
//// Ignore all parameters for C-block
//// DO NOT CHANGE THE NAME OR PARAMETERS OF THIS FUNCTION
//void SimulationEnd(const char *szId, void **reserved_UserData, int reserved_ThreadIndex, void *reserved_AppPtr) {
//
//
//}
