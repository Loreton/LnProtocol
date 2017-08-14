/*
Author:     Loreto Notarantonio
version:    LnVer_2017-08-14_09.39.19

Scope:      Funzione di relay.
                Prende i dati provenienti da una seriale collegata a RaspBerry
                ed inoltra il comando sul bus RS485.
                Provvede ovviamente a catturare la risposta e reinoltrarla a RaspBerry.


*/


//@TODO: bisogna verificarlo
// ################################################################
// # - M A I N     Loop_Relay
// #    - riceviamo i dati da RaspBerry
// #    - facciamo il forward verso rs485
// #    - torniamo indietro la risposta
// ################################################################
void Relay_Main() {
    if (firstRun) {
        // pData->fDisplayData    = true;                // display user/command data

        pData->fDisplayMyData    = true;                // display dati relativi al mio indirizzo
        pData->fDisplayOtherData = true;                // display dati relativi ad  altri indirizzi
        pData->fDisplayRawData = false;                // display raw data
        // pData->fDisplayAllPckt = true;                // display all source/destination packets
    }

    pData->timeout     = 5000;
    pData->rx[DATALEN] = 0;

        // --------------------------------------
        // - ricezione messaggio da RaspBerry
        // --------------------------------------
    byte rCode = recvMsg232(pData);
    // delay(1000)


        // --------------------------------------
        // - se corretto:
        // -    1. inoltra to rs485 bus
        // -    2. attendi risposta
        // -    3. copia comunque su Txdata
        // -    4. Se ricezione OK:
        // -        4a. copia messaggio su TX
        // -        4a. ruota pacchetto verso PI
        // -    5. Se ricezione NOT OK:
        // -        5a. prepara messaggo di errore
        // -        5b. ruota pacchetto verso PI
        // - altrimenti:
        // -    1. ignora
        // --------------------------------------
    if (rCode == LN_OK) {
        Relay_fwdToRs485(pData);
            // qualsiasi esito il msg è pronto da inviare sulla rs232
        Relay_waitRs485Response(pData, 2000);
        sendMsg232(pData);
    }
}


// ################################################################
// # - Forward del messaggio ricevuto da RaspBerry verso RS485
// ################################################################
void Relay_fwdToRs485(RXTX_DATA *pData) {

    copyRxMessageToTx(pData);
        // send to RS-485 bus
    sendMsg485(pData);

}



// ################################################################
// # - Forward del messaggio ricevuto da RS485 verso RaspBerry
// ################################################################
// void Relay_fwdToRaspBerry(RXTX_DATA *pData) {
//     copyRxMessageToTx(pData);
//     sendMsg232(pData);

// }



// ################################################################
// #- riceviamo i dati da rs485
// #-  Se OK allora li torniamo al RaspBerry
// #-  Se ERROR/TIMEOUT ritorniamo errore al RaspBerry
// --------------------------------------
// - se corretto:
// -    1. nothing
// - altrimenti:
// -    1. prepara messaggo di errore
// -    2. set opportunamente gli indirizzi
// - finally:
// -    1. copia Rx to Tx
// -    2. ritorna rCode
// --------------------------------------
// ################################################################
byte Relay_waitRs485Response(RXTX_DATA *pData, unsigned long TIMEOUT) {

    pData->timeout = TIMEOUT;

    byte rcvdRCode = recvMsg485(pData);


        // --------------------------------------------------------
        // - vuol dire che lo slave non ha risposto
        // - o comunque ci sono stati errori nella trasmissione
        // --------------------------------------------------------
    if (rcvdRCode != LN_OK) {
        // -----------------------------------------
        // - Prepariamo il messaggio di errore
        // - lo scriviamo su ->rx
        // - perché poi sarà copiato su ->tx
        // -----------------------------------------
        pData->rx[SENDER_ADDR]      = pData->rx[DESTINATION_ADDR];
        pData->rx[DESTINATION_ADDR] = 0;

                      //-- 01234567
        char errorMsg[] = "ERROR: ........ occurred...!";
        const char *ptr = errMsg[rcvdRCode];

        // copiamo il codice errore nei [....]
        for (byte i=6; *ptr != '\0'; i++, ptr++)
            errorMsg[i] = *ptr;

        setCommandData(pData->rx, errorMsg);

    }


    copyRxMessageToTx(pData);
    return rcvdRCode;
}