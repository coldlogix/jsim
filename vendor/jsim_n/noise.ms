.ps 12
.nr PS 12
.vs 14
.nr VS 14
.ND
.EQ
delim $$
gsize 12
gfont R
define hbar '{ h back 70 up 35 - ~ }'
.EN
.TL
Noise supplement JSIM Preliminary Version User's Guide
.AU
J S Satchell
.AI
DRA(Malvern)
St. Andrews Rd.,
Worcs, UK
WR14 3PS
.sp
.NH
Introduction
.PP
JSIM (Josephson SIMulator) is a circuit simulation program for
Josephson circuits. The noise extension adds two new device types,
and is intended to be a strict superset of standard JSIM, documented in the file manual.ms.
.PP
Modifications, makefile, awk script, this document and the contents of the test directory are
.PP
(c)British Crown copyright January 1997/DERA.            
.PP
Permission to use, copy, modify, and distribute this software   
for any purpose without fee is hereby granted, provided that the 
above copyright notice appears in all copies. The copyright  
holders make no representations about the suitability of this  
software for any purpose. It is provided "as is" without express  
or implied warranty. No liability is accepted by the copyright   
holder for any use made of this software    
.NH
Circuit Description
.NH 2
Independent Voltage Sources
.NH 3
Noise Sources
.RS
.RS
.LP
General form : VXXXX N1 N2 NOISE(VO VA TSTEP TD)
.LP
Example : V1 1 0 NOISE(0 1P 1PS 0PS)
.LP
Note VO must be zero. VA is the spectral amplitude density of the noise 
source in Volts/root(Hz). TSTEP is a the maximum time step that will be used, this provides an opportunity to limit therrors in the stochastic algorithm. 1pS seems to be reasonable for HTS junction parameters. TD is a time delay before 
noise starts.
.NH 2
Independent Current Sources
.NH 3
Noise Sources
.RS
.RS
.LP
General form : IXXXX N1 N2 SIN(IO IA TSTEP TD)
.LP
Example : I1 1 0 NOISE(0 1P 1PS 0PS)
.LP
Note IO must be zero. IA is the spectral amplitude density of the noise 
source in Amps/root(Hz). TSTEP is a the maximum time step that will be used, this provides an opportunity to limit therrors in the stochastic algorithm. 1pS seems to be reasonable for HTS junction parameters. TD is a time delay before 
noise starts.
.RS
BUGS
.PP
If you find any problems with the stochastic extension, please send e-mail to 
satchell@dra.hmg.gb, include the input deck that causes
the problem and a description of the problem. Warning: I may ignore you, be too
busy or be unable to help.