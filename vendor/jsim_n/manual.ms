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
JSIM Preliminary Version User's Guide
.AU
E. S. Fang and T. Van Duzer
.AI
Department of Electrical Engineering and Computer Science
University of California
Berkeley, CA 94720
.sp
.NH
Introduction
.PP
JSIM (Josephson SIMulator) is a circuit simulation program for
Josephson circuits.
Circuits may contain resistors, capacitors, inductors, mutual
inductors, independent voltage and current sources, lossless
transmission lines and
Josephson junctions.
At present time, only transient analysis is allowed.
.PP
The input format of JSIM is quite similar to SPICE.
If you are not familiar with SPICE, it is recommended that
you read the SPICE user guide also.
.NH
Circuit Description
.NH 2
Resistors
.RS
.RS
.LP 
General form : RXXXX N1 N2 VALUE
.LP
Example : RC1 12 9 1KOHM
.RE
.RE
.NH 2
Capacitors
.RS
.RS
.LP
General form : CXXXX N1 N2 VALUE <IC=VALUE>
.LP
Example : C1 10 11 1PF
.LP
Initial value may be specified, but currently it is
ignored.  This applies to ALL initial values.
.RE
.RE
.NH 2
Inductors
.RS
.RS
.LP
General form : LXXXX N1 N2 VALUE <FCHECK> <IC=VALUE>
.LP
Example : L2 1 0 2.3PH FCHECK 
.LP
If FCHECK (flux check) is specified, 
JSIM will keep track of the change of
flux through the inductor.  It is recommended particularly for
phase mode circuit. In each superconductive loop, at least
one inductor should be considered for FCHECK option. 
.RE
.RE
.NH 2
Mutual Inductors
.RS
.RS
.LP
General form : KXXXX LXXXX LYYYY VALUE
.LP
Example : K1 L1 L2 0.9
.RE
.RE
.NH 2
Independent Voltage Sources
.NH 3
Sinusoidal Sources
.RS
.RS
.LP
General form : VXXXX N1 N2 SIN(VO VA FREQ TD THETA)
.LP
Example : V1 1 0 SIN(0 1MV 100MEGHZ 0US 0)
.LP
Note VO must be zero.
.RE
.RE
.NH 3
Pulse Sources
.RS
.RS
.LP
General form : VXXXX N1 N2 PULSE(V1 V2 TD TR TF PW PER)
.LP
Example : V2 2 0 PULSE(0MV 1MV 0PS 2PS 2PS 10PS 50PS)
.LP
Note V1 must be zero.
.RE
.RE
.NH 3
Piece-wise Linear Sources
.RS
.RS
.LP
General form : VXXXX N1 N2 PWL(T0 V0 T1 V1 ....)
.LP
Example : V3 3 0 PWL(0PS 0MV 1PS 1MV)
.LP
Note T0 and V0 must be zero.
.RE
.RE
.NH 2
Independent Current Sources
.NH 3
Sinusoidal Sources
.RS
.RS
.LP
General form : IXXXX N1 N2 SIN(IO IA FREQ TD THETA)
.LP
Example : I1 1 0 SIN(0 1MA 100MEGHZ 0US 0)
.LP
Note IO must be zero.
.RE
.RE
.NH 3
Pulse Sources
.RS
.RS
.LP
General form : IXXXX N1 N2 PULSE(I1 I2 TD TR TF PW PER)
.LP
Example : I2 2 0 PULSE(0MA 1MA 0PS 2PS 2PS 10PS 50PS)
.LP
Note I1 must be zero.
.RE
.RE
.NH 3
Piece-wise Linear Sources
.RS
.RS
.LP
General form : IXXXX N1 N2 PWL(T0 I0 T1 I1 ....)
.LP
Example : I3 3 0 PWL(0PS 0MA 1PS 1MA)
.LP
Note T0 and I0 must be zero.
.RE
.RE
.NH 2
Josephson Junctions
.RS
.RS
.LP
General form : BXXXX N1 N2 MODNAME <AREA> <CONDEV=DEVNAME>
<IC=V0,PHI0>
.LP
Example : B1 2 3 JJMOD1 AREA=1.5 CONDEV=L2
.LP
MODNAME is the model name, 
CONDEV is used to simulate the modulation of critical current
due to magnetic field, DEVNAME can only be inductors, voltage and
current sources.
.RE
.RE
.NH 2
Transmission Line 
.RS
.RS
.LP
General form : TXXXX N1 N2 N3 N4 LOSSLESS <Z0=VALUE> <TD=VALUE>
.LP
Example : T1 1 0 2 0 LOSSLESS Z0=50 TD=100PS
.LP
N1 and N2 are nodes for port 1, and N3 and N4 are nodes for
port 2.
Default Z0=50ohm, TD=1sec.
.RE
.RE
.NH 2
Subcircuit Calls
.RS
.RS
.LP
General form : XYYYY SUBDEFNAME N1 N2 .....
.LP
Example : X1 SGA 1 3 4 5
.RE
.RE
.NH
Subcircuit Definition
.RS
.RS
.LP
General form : .SUBCKT SUBDEFNAME N1 N2 ......
.LP
General form : <circuit elements>
.LP
General form : .ENDS
.LP
Example : .SUBCKT TEST 1 2 3
.LP
Example : R1 1 2 3K
.LP
Example : C1 2 3 3PF
.LP
Example : .ENDS
.RE
.RE
.NH
Model Specification
.NH 2
Josephson Models
.RS
.RS
.LP
General form : .MODEL MNAME JJ(<PARAM=VALUE>,....)
.LP
Example : .MODEL JJMOD1 JJ(VG=2.5MV, CAP=0.6PF, ICRIT=100UA)
.LP
RTYPE : quasiparticle model, can be zero or one.
Zero is for zero conductance, and one is for piece-wise
linear conductance curve, default is 0. 
.LP
CCT : control current type, can be zero or one. 
Zero is for no control current, and one is for sine x over x,
default is 0.
.LP
VG : gap voltage, default is 2.8mV.
.LP
DELV : gap transition voltage, default is 0.1mV.
.LP
ICON : control current scale, default is 1mA.
.LP
R0 : subgap resistance, default is 30ohm.
.LP
RN : normal resistance, default is 5ohm.
.LP
CAP : junction capacitance, default is 2.5pf
.PP
ICRIT : critical current, default is 1mA.
.RE
.RE
.NH
Transient Analysis Specification
.RS
.RS
.LP
General form : .TRAN PRSTEP TSTOP <TSTART> <MAXTSTEP>
.LP
Example : .TRAN 1PS 100PS 20PS 0.5PS
.LP
PRSTEP is the printing step.
.LP
TSTOP is the stop time.
.LP
TSTART is the starting time for printing, default is 0.
.LP
MAXTSTEP is the maximum internal time step, default is 1ps.
.RE
.RE
.NH
Print and File Specifications
.RS
.RS
.LP
General form : .FILE FILENAME
.LP
General form : .PRINT PRTYPE PRNAME <PART>
.LP
Example : .FILE OUT1
.LP
Example : .PRINT NODEV 2 0
.LP
Example : .PRINT DEVV X1_X2_B1
.LP
Example : .FILE OUT2
.LP
Example : .PRINT DEVI B2 JJTOTAL
.LP
Example : .PRINT PHASE B3
.LP
Example : .PRINT DEVV T1 PORT1
.LP
Example : .PRINT DEVI T1
.LP
To print out device voltage or current of a subcircuit
element, just expand the element name by the subcircuit call
name.
In the above examples, X1_X2_B1, means B1 belong to subcircuit
call X2 which is in turn called by X1.
This eliminates the need for a long node list in the
subcircuit definition in order to print out values associated
with subcircuit elements as in SPICE.
.LP
JJTOTAL : total current.
.LP
JJJOSEPH : Josephson current.
.LP
JJCAP : current due to capacitive effect.
.LP
JJRESIS : quasi-partical current.
.LP
JJALL : print all currents in the order specified.
.LP
PORT1 : print port1 of transmission line.
.LP
PORT2 : print port2 of transmission line, no port
specification prints both ports.
.RE
.RE
.NH
Option Specifications
.RS
.RS
.LP
General form : .OPTIONS <PARAM, ....>
.LP
Example : .OPTIONS RELTOL=0.01 MAXPHISTEP=1.5
.sp
.LP
RELTOL=VALUE : relative tolerance, default is 0.001.
.LP
PHITOL=VALUE : absolute tolerance for phase, default is
0.0001, (too small, set it to 0.01).
.LP
VNTOL=VALUE : absolute voltage tolerance, default is 0.1 uV,
only in effect when LTE option is used.
.LP
INTOL=VALUE : absolute current tolerance, default is 0.1 uA,
only in effect when LTE option is used.
.LP
MAXPHISTEP=VALUE : maximum phase change allowed in one time step,
default is 1.5.
.LP
MAXFLUXSTEP=VALUE : maximum flux change in inductor allowed in
one time step.
Only applies to those inductors with FCHECK flag specified.
Default is 0.5e-15.
.LP
LTE : check for local truncation error in choosing step size,
default no LTE.
.LP
IGWARN : ignore warning and proceed with simulation.
.LP
NUMDGT=VALUE : number of digits to be printed, default is 3.
.RE
.RE
.NH
Special Files
.RS
.RS
.LP
JSIM will generate some special files.
All the speical files start with .jsim.
They are devlist, devname, model, nodemap and subdef.
.RE
.RE
.NH
CAUTION
.PP
This is a preliminary version of the program.
Many parameters have no defaults, and have to be specified.
.NH
BUGS
.PP
If you find any problems, please send e-mail to 
esfang@argon.berkeley.edu, include the input deck that causes
the problem and a description of the problem.
Also you may call (415) 642-0502 and contact Emerson Fang, or
write to Prof. Ted Van Duzer at the above address.
