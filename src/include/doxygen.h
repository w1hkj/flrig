// ----------------------------------------------------------------------------
//      doxygen.h
//
// Copyright (C) 2013-2021
//              John Phelps, KL4YFD
//
// This file is part of fldigi.
//
// fldigi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

/*
	This include file is not indended to be included by any source file in Fldigi !
	
	It is used only for DOXYGEN automatic-documentation generation.
	Used for Fldigi file: /scripts/srcdoc/gen_doxygen_srcdoc.sh 
	Please put only comments & Doxygen info in this file.
*/


/** \mainpage Flrig Sourcecode Interactive Doxygen Documentation : 000GIT_REVISION000
 <div align="center"><img src="fldigi-psk.png" ></div>

 \section intro Introduction

  Welcome to the Flrig Interactive Doxygen Sourcecode Documentation!
  Here you'll find information useful for developing and debugging Flrig. 
  
  Flrig - File transfers over radio using the Amateur Multicast Protocol v2.0


  <BR>
  \section download To Download Flrig :
  <UL>
  <LI> To pull latest source with developer access: <B> git clone ssh://<YOUR-LOGIN>\@git.code.sf.net/p/fldigi/flrig fldigi-flrig</B></LI>
  <LI> To pull latest source with developer access behind proxy/firewall: <B> git clone https://<YOUR-LOGIN>\@git.code.sf.net/p/fldigi/flrig fldigi-flrig</B></LI>
  <LI> To pull latest source with no account, read-only: <B> git clone git://git.code.sf.net/p/fldigi/flrig fldigi-flrig</B></LI>
  </UL>
  
  
  <BR>
  \section generate Generating This Documentation :
  <OL>
  <LI>Download the Flrig sourcecode as described above</LI>
  <LI>Unpack the sourcecode to a Linux / Unix / MacOS environment (Debian or Ubuntu preferred)</LI>
  <LI>Checkout the desired branch or release using the command - <B>git checkout</B></LI>
  <LI>Open a terminal and go to sourcecode sub-directory - <B>scripts/srcdoc</B></LI>
  <LI>In this directory, run command - <B>./gen_doxygen_srcdoc.sh run</B></LI>
    <OL>
    <LI type="a">If any needed binaires are missing, you will be notified.</LI>
    <LI type="a">On Debian / Ubuntu binaries can be installed by command <B>./gen_doxygen_srcdoc.sh install</B></LI>
    <LI type="a">On other systems, the following binaries must be installed manually:</LI>
        <UL>
        <LI>cppcheck dot doxygen mscgen gitstats</LI>
        </UL>
    </OL>
  </OL>
  

  <BR>
  \section cppcheck_results CppCheck Static Program Analysis Results :
  
  <UL>
  <LI><B><A HREF="results/TOTALS.txt">TOTALS</A></B></LI>
  <LI><A HREF="results/error.txt">Errors</A></LI>
  <LI><A HREF="results/warning.txt">Warnings</A></LI>
  <LI><A HREF="results/style.txt">Code-Style Issues</A></LI>
  <LI><A HREF="results/performance.txt">Performance Issues</A></LI>
  <LI><A HREF="results/portability.txt">Portability Issues</A></LI>
  <LI><A HREF="results/information.txt">Information</A></LI>
  <LI><B>Inconclusive tests did not FAIL, but also did not PASS</B></LI>
  <LI><A HREF="results/error_inconclusive.txt"><I>Inconclusive Errors</I></A></LI>
  <LI><A HREF="results/warning_inconclusive.txt"><I>Inconclusive Warnings</I></A></LI>
  <LI><A HREF="results/style_inconclusive.txt"><I>Inconclusive Code-Style Issues</I></A></LI>
  <LI><A HREF="results/performance_inconclusive.txt"><I>Inconclusive Performance Issues</I></A></LI>
  <LI><A HREF="results/portability_inconclusive.txt"><I>Inconclusive Portability Issues</I></A></LI>
  <LI><A HREF="results/information_inconclusive.txt"><I>Inconclusive Information</I></A></LI>
  </UL>

  
  <BR>
  \section git Patches for ALL Git Commits :
  <UL>
  <LI><A HREF="__git/">Patches for ALL commits</A></LI>
  <LI><A HREF="__git/gitlog.txt">git-log for ALL git commits</A></LI>
  </UL>


  <BR>
  \section license License

 	Flrig is free software; you can redistribute it and/or modify
 	it under the terms of the GNU General Public License as published by
 	the Free Software Foundation; either version 3 of the License, or
 	(at your option) any later version.

 	fldigi is distributed in the hope that it will be useful,
 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 	GNU General Public License for more details.

 	You should have received a copy of the GNU General Public License
 	along with this program.  If not, see http://www.gnu.org/licenses

 
*/
