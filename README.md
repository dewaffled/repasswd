# repasswd
Windows domain password rearm utility

Changing domain password of current user 25 times and resetting it back to original value.

Retrieving current user credentials with [GetCurrentUserAndDomain](https://support.microsoft.com/en-us/help/111544/how-to-retrieve-current-user-and-domain-names-on-windows-nt--windows-2) and updating with `NetUserChangePassword`.
