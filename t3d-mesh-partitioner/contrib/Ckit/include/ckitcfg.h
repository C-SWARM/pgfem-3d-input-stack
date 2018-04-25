#ifndef DID_CKITCFG_H
#define DID_CKITCFG_H


#ifdef MSDOS

#define STATIC static		
#define STANDARD_MALLOC 1       /* use only standard facilities; note */
				/* that a hack concerning free() */
				/* facility is used. */
#else  /* !MSDOS */
       /* UNIX */

#define STATIC static

#ifdef STANDARD_MALLOC
#   if STANDARD_MALLOC == 0
#      undef STANDARD_MALLOC   /* We signal that we DO NOT want */
                               /* STANDARD_MALLOC by -DSTANDARD_MALLOC=0 */
#   endif
#else
#   define STANDARD_MALLOC 1   /* This is to go safe;  see description */
                               /* of problems with the memory management */
                               /* by Zimmermann in the documentation */
#endif

#endif /* UNIX */

#endif /* DID_CKITCFG_H */
