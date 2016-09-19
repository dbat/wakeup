# wakeup
Wake up computers on connected LAN

    Copyright (c) 2003-2011
    Adrian H, Ray AF & Raisa NF of PT SOFTINDO, Jakarta.
    Email: aa _AT_ softindo.net
    All rights reserved.

    Version: 0.1.1 build 003
    Created: 2004.02.01
    Revised: 2007.07.12
    
      Compiled with Borland's BCC 5.5 (Freeware) +lib: ws2_32.lib

 SYNOPSYS:
 
        Wake up computers on connected LAN

 USAGE:
 
        wakeup.exe mac-addresses...

 ARGUMENTS:
 
        mac-address: 1 or more mac address of remote computer

 NOTES:
 
        MAC separator can be any of [ : - . _ / ]
        
          (semicolon, dash, dot, underscore or slash)
          
        Thus, "ab/cd:00.1-2_3" would be a VALID mac address.

        Any missing digit/elements will be interpreted as 0

 EXAMPLES:

  - Wake up a single computer:
   - 
        wakeup.exe 00:aa:bb:cc:dd:ee

  - Wake up computers with several separator's style:
   - 
        wakeup.exe 00:aa:bb:cc:dd:ee 00-01-02-03-04-05 11.22/33..

  - Wake up several computers with typos:
   - 
        wakeup.exe 701:::b3b:5c4c 100-011-102-031

     The program will pick the last 2 characters per-element,
     
     and fills up missing digits with zero. In this case,
     
     it would be equal with typing command:
     
      wakeup.exe 01:00:00:3b:4c:00 00-11-02-31-00-00
      
      
