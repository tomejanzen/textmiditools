s; $Author: janzent $
; $Log: schuqnt2.m4,v $
; Revision 1.1.1.1  2004/12/28 19:40:42  janzent
; import moremiscprog and textmidi
;
; Revision 1.13  2003/11/09 18:23:20  root
; add ppp macro
;
; Revision 1.12  2003/10/17 01:28:03  root
; fix accent quote on temposighrit
; and temposigh
;
; Revision 1.11  2003/09/30 01:08:24  root
; some of jenny stirling notes from 22 sep 2003
;
; Revision 1.10  2003/08/07 01:04:11  root
; long tones, decresc, cresc, etc.
;
; Revision 1.9  2003/04/30 00:39:17  root
; nuanced tempo
;
; Revision 1.8  2003/04/24 00:39:11  root
; added pan and program macros, intmidi macro
;
; Revision 1.7  2003/03/23 02:31:03  root
; final before repeats
;
; Revision 1.6  2003/03/17 02:59:25  root
; goodnight
;
; Revision 1.5  2003/03/15 16:50:11  root
; almost all roughed in
;
; Revision 1.4  2003/03/15 06:14:35  root
; changed something
;
; Revision 1.3  2003/03/11 02:03:38  root
; add piano right
;
; Revision 1.2  2003/03/09 21:40:55  root
; v1, v2, va roughed in
;
; Revision 1.1  2003/03/08 03:05:28  root
; Initial revision
;
;
; $Date: 2004/12/28 19:40:42 $
; $Header: /mnt/hdc5/CVSROOT/thomas/moremiscprog/textmidi/3dschuqnt/schuqnt2.m4,v 1.1.1.1 2004/12/28 19:40:42 janzent Exp $
; $Id: schuqnt2.m4,v 1.1.1.1 2004/12/28 19:40:42 janzent Exp $
; $Locker:  $
; $Name:  $
; $Revision: 1.1.1.1 $
; $RCSfile: schuqnt2.m4,v $
; $Source: /mnt/hdc5/CVSROOT/thomas/moremiscprog/textmidi/3dschuqnt/schuqnt2.m4,v $
; $State: Exp $
; Schumann Quintet second movement top level.
;
changecom(`;')
;Schumann Quintet in textmidi language © 2003 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭
include(`schuqntutils.m4')

define(`realtempo2',`132')
define(`agitato',eval((realtempo2*11)/10))

define(`agitato1',eval((realtempo2*9)/10))
define(`temposigh',`END_LAZY TEMPO eval((realtempo2*4)/5) DELAY 480 TEMPO realtempo2 DELAY 480 LAZY')
define(`temposighrit',`END_LAZY TEMPO eval( ( $1 * 4) / 5) DELAY 480 TEMPO $1 DELAY 480 LAZY')

ifdef(`clicktrack',`FILEHEADER 10 ticks',`FILEHEADER 9 ticks')

include(`schuqnt2tempo.txt')
include(`schuqnt2v1.txt')
include(`schuqnt2v2.txt')
include(`schuqnt2va.txt')
include(`schuqnt2vc.txt')
include(`schuqnt2vc2.txt')
include(`schuqnt2pr.txt')
include(`schuqnt2pl.txt')
include(`schuqnt2pt.txt')
ifdef(`clicktrack',`include(`schuqnt2click.txt')')

