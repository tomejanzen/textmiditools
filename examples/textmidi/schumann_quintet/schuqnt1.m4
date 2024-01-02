; $Author: janzent $
; $Log: schuqnt1.m4,v $
; Revision 1.1.1.1  2004/12/28 19:40:42  janzent
; import moremiscprog and textmidi
;
; Revision 1.12  2003/11/09 18:23:20  root
; add ppp macro
;
; Revision 1.11  2003/10/30 02:00:05  root
; try to fix constructive interference of unisons octaves
;
; Revision 1.10  2003/08/07 01:04:11  root
; long tones, decresc, cresc, etc.
;
; Revision 1.9  2003/08/01 01:42:55  root
; using long tones, decresc, cresc pretty much done. fixed first big cadence
;
; Revision 1.8  2003/07/06 22:04:34  root
; lesson notes
;
; Revision 1.7  2003/05/23 01:55:52  root
; fix staccato implied in first theme at all appearances
;
; Revision 1.6  2003/05/05 00:15:38  root
; pedals, dynamics
;
; Revision 1.5  2003/05/03 01:16:59  root
; introduce performance practice pedalling
;
; Revision 1.4  2003/04/24 00:39:11  root
; added pan and program macros, intmidi macro
;
; Revision 1.3  2003/04/21 23:53:55  root
; changecom ;
;
; Revision 1.2  2003/03/04 03:16:35  root
; done
;
; Revision 1.1  2003/02/16 20:23:18  root
; Initial revision
;
; Revision 1.37  2003/02/16 19:00:48  root
; change f to forte; dynamics to first repeat  sign
;
; Revision 1.36  2003/02/16 02:01:49  root
; dynamics  through 17
;
; Revision 1.35  2003/02/16 01:15:33  root
; change comment char
;
; Revision 1.34  2003/02/15 01:59:41  root
; started dynamics
;
; Revision 1.33  2003/02/15 01:55:43  root
; ritartandi are probably correct
;
; Revision 1.32  2003/02/15 00:12:09  root
; piano lh probably corrected
;
; Revision 1.31  2003/02/14 02:16:07  root
; draft left hand
;
; Revision 1.30  2003/02/14 00:44:06  root
; cleaned up piano rh to end
;
; Revision 1.29  2003/02/14 00:14:31  root
; roughed in piano rh to end
;
; Revision 1.28  2003/02/11 01:27:24  root
; cleaned up everything to end, but piano not done
;
; Revision 1.27  2003/02/10 02:00:39  root
; fix a couple errors found by searching
;
; Revision 1.26  2003/02/10 01:54:08  root
; compiles without error.
;
; Revision 1.25  2003/02/10 00:46:13  root
; cello done roughed in
;
; Revision 1.24  2003/02/09 23:52:37  root
; finished viola
;
; Revision 1.23  2003/02/09 23:17:14  root
; vinished violin 2
;
; Revision 1.22  2003/02/09 20:58:39  root
; finished violin 1
;
; Revision 1.21  2003/02/09 16:39:04  root
; fix ab 4
;
; Revision 1.20  2003/02/09 15:58:02  root
; not too bad through 174 plus
;
; Revision 1.19  2003/02/09 01:08:04  root
; filled in through 178 but needs fixes
;
; Revision 1.18  2003/02/08 02:47:46  root
; through repeat is done for notes and rhythms. whew.
;
; Revision 1.17  2003/02/06 01:28:11  root
; remove comments; textmidi may have comment problems
;
; Revision 1.16  2003/02/02 21:30:17  root
; use new PAN keyword; remove DLEAY 0, no longer needed.
;
; Revision 1.15  2003/01/29 02:41:44  root
; added some piano before repeat
;
; Revision 1.14  2003/01/25 01:48:20  root
; finished piano left hand through repeat, might be an eighth off
;
; Revision 1.13  2003/01/25 01:08:21  root
; took piano rh through repeat
;
; Revision 1.12  2003/01/24 01:59:32  root
; took piano rh almost to repeat, m. 99
;
; Revision 1.11  2003/01/22 03:09:33  root
; cello through repeaat sign
;
; Revision 1.10  2003/01/19 17:32:04  root
; added some cello
;
; Revision 1.9  2003/01/19 16:35:29  root
; viola thru repeat and beyond
;
; Revision 1.8  2003/01/19 15:44:41  root
; 1rst and 2nd thorugh repeat and beyond
;
; Revision 1.7  2003/01/19 15:01:52  root
; took 1rst through repeat sign and also the repeat itself
;
; Revision 1.6  2003/01/19 03:40:27  root
; took 2d vln further
;
; Revision 1.5  2003/01/19 03:25:45  root
; fix missing delay before end of track
;
; Revision 1.4  2003/01/19 03:11:22  root
; may have a problem, but violin done to repeat
;
; Revision 1.3  2003/01/18 18:36:01  root
; use lazy mode, clean up mistakes
;
; Revision 1.2  2003/01/16 02:20:41  root
; fixed a few mistakes, replaced dots until I may fix dots
;
; Revision 1.1  2003/01/05 18:47:03  root
; Initial revision
;
; $Date: 2004/12/28 19:40:42 $
; $Header: /mnt/hdc5/CVSROOT/thomas/moremiscprog/textmidi/3dschuqnt/schuqnt1.m4,v 1.1.1.1 2004/12/28 19:40:42 janzent Exp $
; $Id: schuqnt1.m4,v 1.1.1.1 2004/12/28 19:40:42 janzent Exp $
; $Locker:  $
; $Name:  $
; $Revision: 1.1.1.1 $
; $RCSfile: schuqnt1.m4,v $
; $Source: /mnt/hdc5/CVSROOT/thomas/moremiscprog/textmidi/3dschuqnt/schuqnt1.m4,v $
; $State: Exp $
ifdef(`clicktrack',`FILEHEADER 10 240',`FILEHEADER 9 240')

changecom(`;')
;Schumann Quintet in textmidi language © 2003 by Thomas E. Janzen is licensed under CC BY 4.0 🅯🅭
include(`schuqntutils.m4')

include(`schuqnt1tempo.txt')
include(`schuqnt1v1.txt')
include(`schuqnt1v2.txt')
include(`schuqnt1va.txt')
include(`schuqnt1vc.txt')
include(`schuqnt1pr.txt')
include(`schuqnt1pl.txt')
include(`schuqnt1pt.txt')
include(`schuqnt1ped.txt')
ifdef(`clicktrack',`include(`schuqnt1click.txt')')

