#include "stdafx.h"
#include "userapi.h"

//**********************************************************************
// Fetched from
// https://support.microsoft.com/en-us/help/111544/how-to-retrieve-current-user-and-domain-names-on-windows-nt,-windows-2000,-or-windows-xp
//**********************************************************************
//
//  FUNCTION:     GetCurrentUserAndDomain - This function looks up
//                the user name and domain name for the user account
//                associated with the calling thread.
//
//  PARAMETERS:   szUser - a buffer that receives the user name
//                pcchUser - the size, in characters, of szUser
//                szDomain - a buffer that receives the domain name
//                pcchDomain - the size, in characters, of szDomain
//
//  RETURN VALUE: TRUE if the function succeeds. Otherwise, FALSE and
//                GetLastError() will return the failure reason.
//
//                If either of the supplied buffers are too small,
//                GetLastError() will return ERROR_INSUFFICIENT_BUFFER
//                and pcchUser and pcchDomain will be adjusted to
//                reflect the required buffer sizes.
//
//**********************************************************************

BOOL GetCurrentUserAndDomain(PTSTR szUser, PDWORD pcchUser, PTSTR szDomain, PDWORD pcchDomain)
{

    BOOL         fSuccess = FALSE;
    HANDLE       hToken = NULL;
    PTOKEN_USER  ptiUser = NULL;
    DWORD        cbti = 0;
    SID_NAME_USE snu;

    __try {

        // Get the calling thread's access token.
        if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE,
            &hToken)) {

            if (GetLastError() != ERROR_NO_TOKEN)
                __leave;

            // Retry against process token if no thread token exists.
            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY,
                &hToken))
                __leave;
        }

        // Obtain the size of the user information in the token.
        if (GetTokenInformation(hToken, TokenUser, NULL, 0, &cbti)) {

            // Call should have failed due to zero-length buffer.
            __leave;

        }
        else {

            // Call should have failed due to zero-length buffer.
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                __leave;
        }

        // Allocate buffer for user information in the token.
        ptiUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), 0, cbti);
        if (!ptiUser)
            __leave;

        // Retrieve the user information from the token.
        if (!GetTokenInformation(hToken, TokenUser, ptiUser, cbti, &cbti))
            __leave;

        // Retrieve user name and domain name based on user's SID.
        if (!LookupAccountSid(NULL, ptiUser->User.Sid, szUser, pcchUser,
            szDomain, pcchDomain, &snu))
            __leave;

        fSuccess = TRUE;

    }
    __finally {

        // Free resources.
        if (hToken)
            CloseHandle(hToken);

        if (ptiUser)
            HeapFree(GetProcessHeap(), 0, ptiUser);
    }

    return fSuccess;
}
