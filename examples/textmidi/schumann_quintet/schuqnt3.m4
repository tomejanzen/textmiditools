; $Author: janzent $
; $Log: schuqnt3.m4,v $
; Revision 1.1.1.1  2004/12/28 19:40:42  janzent
; import moremiscprog and textmidi
;
; Revision 1.17  2003/11/09 18:23:20  root
; add ppp macro
;
; Revision 1.16  2003/08/07 01:04:11  root
; long tones, decresc, cresc, etc.
;
; Revision 1.15  2003/05/30 09:33:41  root
; added portamento to second trio
;
; Revision 1.14  2003/05/30 00:27:10  root
; add portamento macro
;
; Revision 1.13  2003/05/25 16:42:51  root
; hi
;
; Revision 1.12  2003/05/25 16:41:39  root
; worked on chaeating the quavers to be staccato
;
; Revision 1.11  2003/05/05 00:36:12  root
; pedals dynamics
;
; Revision 1.10  2003/04/24 00:39:11  root
; added pan and program macros, intmidi macro
;
; Revision 1.9  2003/03/15 05:16:08  root
; don't includ click track; it creates a dead track in the midi file
;
; Revision 1.8  2003/03/11 23:36:14  root
; duplicate oct4
;
; Revision 1.7  2003/03/09 21:40:44  root
; done
;
; Revision 1.6  2003/03/04 03:16:40  root
; done
;
; Revision 1.5  2003/03/02 22:19:50  root
; rhythms ok excep t maybe last change to 6/8
;
; Revision 1.4  2003/03/01 02:39:21  root
; added v1
;
; Revision 1.3  2003/02/28 01:21:42  root
; done drafting all piano
;
; Revision 1.2  2003/02/27 02:35:27  root
; some cleaning
;
; Revision 1.1  2003/02/24 01:53:56  root
; Initial revision
;
;
; $Date: 2004/12/28 19:40:42 $
; $Header: /mnt/hdc5/CVSROOT/thomas/moremiscprog/textmidi/3dschuqnt/schuqnt3.m4,v 1.1.1.1 2004/12/28 19:40:42 janzent Exp $
; $Id: schuqnt3.m4,v 1.1.1.1 2004/12/28 19:40:42 janzent Exp $
; $Locker:  $
; $Name:  $
; $Revision: 1.1.1.1 $
; $RCSfile: schuqnt3.m4,v $
; $Source: /mnt/hdc5/CVSROOT/thomas/moremiscprog/textmidi/3dschuqnt/schuqnt3.m4,v $
; $State: Exp $
; Schumann Quintet third movement top level.
;
changecom(`;')
;Schumann Quintet in textmidi language © 2003 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭
include(`schuqntutils.m4')
define(`realtempo3',`138')
define(`tempo68', `eval((3*realtempo3)/2)')
define(`tempotrioa', `eval((19*tempo68)/20)')

ifdef(`clicktrack',`FILEHEADER 9 ticks',`FILEHEADER 8 ticks')

include(`schuqnt3tempo.txt')
include(`schuqnt3v1.txt')
include(`schuqnt3v2.txt')
include(`schuqnt3va.txt')
include(`schuqnt3vc.txt')
include(`schuqnt3pr.txt')
include(`schuqnt3pl.txt')
include(`schuqnt3pt.txt')
ifdef(`clicktrack',`include(`schuqnt3click.txt')')

