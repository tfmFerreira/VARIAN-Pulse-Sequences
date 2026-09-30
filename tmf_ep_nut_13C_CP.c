/*tmf_ep_nut_13C_CP An experiment to measure the nutation frequency using CP
The increment is defined by sw1,

*/

#include "standard.h"
#include "solidstandard.h"

// Define Values for Phasetables

static int table1[4] = {1,3,2,0};           // phXnut 
static int table11[4] = {0,2,0,2};           // phH90  excitation 1H pulse
static int table12[4] = {0,0,1,1};           // phXhx
static int table13[4] = {1,1,1,1};           // phHhx
static int table14[4] = {0,2,1,3};           // phRec

#define phXnut t1
#define phH90 t11
#define phXhx t12
#define phHhx t13
#define phRec t14


void pulsesequence() {

// Define Variables and Objects and Get Parameter Values

   double duty;
   double pwXini,pwXstep,pulseXlength,tHX;
   
   tHX=getval("tHX");
   pwXini= getval("pwXini");
   pwXstep= getval("pwXstep");

   CP hx = getcp("HX",0.0,0.0,0,1);
   strncpy(hx.fr,"dec",3);
   strncpy(hx.to,"obs",3);
   putCmd("frHX='dec'\n");
   putCmd("toHX='obs'\n");

   DSEQ dec = getdseq("H");  // reads the dec seq!


// Dutycycle Protection   

   duty = 4.0e-6 + 3.0*getval("pwH90") + d2  + getval("tHX") + getval("ad") + getval("rd") + at;
   duty = duty/(duty + d1 + 4.0e-6);
   if (duty > 0.1) {
      printf("Duty cycle %.1f%% >10%%. Abort!\n", duty*100);
      psg_abort(1);
   }

// Set Phase Tables

   settable(phXnut,4,table1);
   settable(phH90,4,table11);
   settable(phXhx,4,table12);
   settable(phHhx,4,table13);
   settable(phRec,4,table14);
   setreceiver(phRec);


 
   //  t1 1H evolution time calculation in cycles:   the t1 evolution time is hardcoded in variable  d2 
   float sw1_read;
   sw1_read=getval("sw1");
    
   float dwellt1_read;
   dwellt1_read= 1/sw1_read;
      //printf("dwell_t1_in_exp= %8.6f Hz\n",dwellt1_read);
         
    pulseXlength=pwXini+(pwXstep*((d2/dwellt1_read))) ;   // calculate the loop cycles in real time for t1 dimension
   



   
 // Begin Sequence

   obsunblank(); decunblank(); _unblank34();
   delay(d1);
   sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

   txphase(phXnut); decphase(phH90);
   obspower(getval("tpwr"));  decpower(getval("dpwr")); /* coarse power levels for CP */
   obspwrf(getval("aXhx")); decpwrf(getval("aH90"));  /* fine power levels for CP */   


// H to X Cross Polarization

    decrgpulse(getval("pwH90"),phH90,0.0,0.0);   /* Cross-Polarization*/
    decphase(phHhx);
    _cp_(hx,phHhx,phXhx);


// X pulse

   obspwrf(getval("aX90"));/* fine power levels for variable pulse */   
   rgpulse(pulseXlength,phXnut,0.0,0.0);


// Begin Acquisition

   obsblank(); _blank34();
   _dseqon(dec);
   delay(getval("rd"));
   startacq(getval("ad"));
   acquire(np, 1/sw);
   endacq();
   _dseqoff(dec);
   obsunblank(); decunblank(); _unblank34();
}
                        
  
 

