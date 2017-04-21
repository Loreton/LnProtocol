#!/usr/bin/python3.4
#
# Scope:  Programma per ...........
# modified:  v2017-03-02_14.30.17
#                                               by Loreto Notarantonio
# ######################################################################################
import sys; sys.dont_write_bytecode = True
import os

#import Project as Prj
import Source as Prj
try:
    import LnLib as Ln
except:
    print ('.....trying via ImportLib....')
    Ln = Prj.ImportLib('LnLib', fDEBUG=False)


################################################################################
# - M A I N
################################################################################
if __name__ == "__main__":
    Prj.Version  = 'V01'
    gv           = Ln.LnDict()              # default = _dynamic=False
    gv.Prj       = Prj
    gv.Ln        = Ln
    Prj.name     = 'LnProtocol'
    Prj.prefix   = Prj.name

        # decidiamo quale dict vogliamo usare

        # ===================================================
        # - per iniziare disabilitiamo il LOG
        # ===================================================
    logger = gv.Ln.SetNullLogger()


        # ===================================================
        # - SetUp dell'ambiente
        # ===================================================
    Prj.SetupEnv(gv, fDEBUG=False)
    gv.LnDict = gv.Prj.LnDict     # Lnclass()
    gv.LnDict = gv.Ln.LnDict      # DotMap()


        # ===================================================
        # - Lettura del file ini
        # ===================================================
    iniFile = gv.Ln.ReadIniFile(gv.Prj.iniFileName)
    iniFile.read()
    gv.ini = gv.Ln.LnDict(iniFile.dict)



        # ===================================================
        # - lettura dei parametri di input
        # - Nel caso specifico abbiamo un argomento multiValue
        # -   e quindi passiamo i valori validi per detto argomento.
        # ===================================================
    Input           = Prj.ParseInput(gv, args=sys.argv[1:])
    gv.input  = gv.Ln.LnDict(Input)
    gv.fDEBUG       = gv.input.fDEBUG

    # gv.input.PrintTree(MaxLevel=3, fEXIT=True)





        # ===================================================
        # - SetUp del log
        # ===================================================
    logger = Prj.SetupLog(gv)





        # ===================================================
        # - Inizio applicazione
        # ===================================================

    Prj.Main(gv, gv.input.actionCommand )
    gv.Ln.Exit(0, "completed", printStack=False, stackLevel=9, console=True)
    sys.exit()



