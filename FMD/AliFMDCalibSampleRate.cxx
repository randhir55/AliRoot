/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
/* $Id$ */
/** @file    AliFMDCalibSampleRate.cxx
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Sun Mar 26 18:31:09 2006
    @brief   Per digitizer card pulser calibration 
*/
//____________________________________________________________________
//                                                                          
// This class stores the sample rate (that is, how many times the
// ATLRO's sample each VA1 channel).  In principle these can be
// controlled per half ring, but in real life it's most likely that
// this value will be the same for all detectors.  This value must be
// retrived from DCS or the like. 
//
#include "AliFMDCalibSampleRate.h"	// ALIFMDCALIBGAIN_H
// #include "AliFMDParameters.h"           // ALIFMDPARAMETERS_H
// #include <AliLog.h>
#include "TString.h"
#include "AliFMDDebug.h" // Better debug macros
#include "iostream"

//____________________________________________________________________
ClassImp(AliFMDCalibSampleRate)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif

//____________________________________________________________________
AliFMDCalibSampleRate::AliFMDCalibSampleRate()
  : fRates(AliFMDMap::kMaxDetectors, AliFMDMap::kMaxRings, 2, 1)
  // fRates(3)
{
  // CTOR 
  fRates.Reset(1);
}

//____________________________________________________________________
AliFMDCalibSampleRate::AliFMDCalibSampleRate(const AliFMDCalibSampleRate& o)
  : TObject(o), fRates(o.fRates)
{
  // Copy ctor 
}

//____________________________________________________________________
AliFMDCalibSampleRate&
AliFMDCalibSampleRate::operator=(const AliFMDCalibSampleRate& o)
{
  // Assignment operator 
  fRates     = o.fRates;
  return (*this);
}

//____________________________________________________________________
void
AliFMDCalibSampleRate::Set(UShort_t det, Char_t ring, 
			   UShort_t sector, UShort_t, UShort_t rate)
{
  // Set values.  Strip argument is ignored 
  UInt_t nSec  = (ring == 'I' ? 10 : 20);
  UInt_t board = sector / nSec;
  fRates(det, ring, board, 0) = rate;
  
}

//____________________________________________________________________
UShort_t
AliFMDCalibSampleRate::Rate(UShort_t det, Char_t ring, 
			    UShort_t sec, UShort_t) const
{
  // Get the sample rate 
  UInt_t nSec  = (ring == 'I' ? 10 : 20);
  UInt_t board = sec / nSec;
  AliFMDDebug(10, ("Getting sample rate for FMD%d%c[%2d,0] (board %d)", 
		    det, ring, sec, board));
  return fRates(det, ring, board, 0);
}
//____________________________________________________________________
void 
AliFMDCalibSampleRate::WriteToFile(ofstream &outFile)
{
  outFile.write("# SampleRate \n",14);
  for(Int_t det=1;det<=3;det++) {
    UShort_t FirstRing = (det == 1 ? 1 : 0);
    for (UShort_t ir = FirstRing; ir < 2; ir++) {
      Char_t   ring = (ir == 0 ? 'O' : 'I');
      // UShort_t nsec = (ir == 0 ? 40  : 20);
      
      for(UShort_t board = 0; board < 2;  board++)  {
	outFile << det                     << ','
		<< ring                    << ','
		<< board                   << ','
		<< Rate(det,ring,board)    << "\n";
	  

      }
    }
  }
 
      
}
//____________________________________________________________________
void 
AliFMDCalibSampleRate::ReadFromFile(ifstream &inFile)
{
  TString line;
  Bool_t readData=kFALSE;

  while(line.ReadLine(inFile)) {
    if(line.Contains("samplerate",TString::kIgnoreCase)) {
      readData = kTRUE;
      break;
    }
    
  }
  
  UShort_t det, board;
  Char_t ring;
  UShort_t sampleRate;
  Int_t thisline = inFile.tellg();
  Char_t c[3];
  
  while( readData ) {
    thisline = inFile.tellg();
    line.ReadLine(inFile);
    if(line.Contains("# ",TString::kIgnoreCase)) {
      readData = kFALSE;
      continue;
    }
    
    inFile.seekg(thisline);
    inFile     >> det          >> c[0]
	       >> ring         >> c[1]
	       >> board        >> c[2]
	       >> sampleRate;
    
    UInt_t nSec  = (ring == 'I' ? 10 : 20);
    UShort_t sector = board*nSec;
    Set(det,ring,sector,0,sampleRate);
    
    
  }
  
  inFile.seekg(0);
 
  
}

//____________________________________________________________________
//
// EOF
//
