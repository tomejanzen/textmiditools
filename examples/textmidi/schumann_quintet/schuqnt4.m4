; $Author: janzent $
; $Log: schuqnt4.m4,v $
; Revision 1.1.1.1  2004/12/28 19:40:42  janzent
; import moremiscprog and textmidi
;
; Revision 1.14  2003/11/09 18:23:20  root
; add ppp macro
;
; Revision 1.13  2003/10/28 02:06:17  root
; all done before final finesses
;
; Revision 1.12  2003/10/25 01:58:03  root
; to around 180
;
; Revision 1.11  2003/10/23 01:55:08  root
; more final notes to around 120
;
; Revision 1.10  2003/10/22 02:13:35  root
; Jenny's last notes through m47
;
; Revision 1.9  2003/08/02 00:42:00  root
; long tonews done
;
; Revision 1.8  2003/05/26 20:46:19  root
; added staccato octaves
;
; Revision 1.7  2003/04/24 00:39:11  root
; added pan and program macros, intmidi macro
;
; Revision 1.6  2003/04/19 01:39:27  root
; half done with dynamics
;
; Revision 1.5  2003/04/18 10:55:54  root
; fixes
;
; Revision 1.4  2003/04/14 01:52:07  root
; finished 4th mmt drft
;
; Revision 1.3  2003/04/13 00:56:18  root
; fixed for piano
;
; Revision 1.2  2003/04/07 00:34:15  root
; weekend work
;
; Revision 1.1  2003/04/03 02:27:05  root
; Initial revision
;
;
; $Date: 2004/12/28 19:40:42 $
; $Header: /mnt/hdc5/CVSROOT/thomas/moremiscprog/textmidi/3dschuqnt/schuqnt4.m4,v 1.1.1.1 2004/12/28 19:40:42 janzent Exp $
; $Id: schuqnt4.m4,v 1.1.1.1 2004/12/28 19:40:42 janzent Exp $
; $Locker:  $
; $Name:  $
; $Revision: 1.1.1.1 $
; $RCSfile: schuqnt4.m4,v $
; $Source: /mnt/hdc5/CVSROOT/thomas/moremiscprog/textmidi/3dschuqnt/schuqnt4.m4,v $
; $State: Exp $
; Schumann Quintet Fourth movement top level.
;
changecom(`;')
;Schumann Quintet in textmidi language © 2003 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭
include(`schuqntutils.m4')

define(`realtempo4',`252')
define(`tempo2',`eval((realtempo4*80)/100)')

ifdef(`clicktrack',`FILEHEADER 9 ticks',`FILEHEADER 8 ticks')

include(`schuqnt4tempo.txt')
include(`schuqnt4v1.txt')
include(`schuqnt4v2.txt')
include(`schuqnt4va.txt')
include(`schuqnt4vc.txt')
include(`schuqnt4pr.txt')
include(`schuqnt4pl.txt')
include(`schuqnt4pt.txt')
ifdef(`clicktrack',`include(`schuqnt4click.txt')')

