# Ubuntu Pastebin Poster
Ubuntu Pastebin Poster is widget that submits code to <https://paste.ubuntu.com> and displays its return URL. You can manually select the location of the file, or you can drag the file into the window. Click the **Post** button, wait a moment and you will see the URL returned from the site. In addition, you can also add this program to the right mouse button menu, so that you can choose a file to send quickly at any time. Of course, if your IDE supports external tools, you can also add this program to your IDE.


![Ubuntu Pastebin Poster.png](https://s2.loli.net/2022/01/23/NbOegxdTc39RIiM.png)


## Requirements
|     |   |
|  ----  | ----  |
| **Minimum supported client**  | Windows XP, Windows 2000 Professional with SP3 [desktop apps only] |
| **Minimum supported server**  | Windows Server 2003, Windows 2000 Server with SP3 [desktop apps only] |
| **Target Platform**  | Windows |
| **Redistributable** | WinHTTP 5.1 and Internet Explorer 5.01 or later on Windows XP and Windows 2000. |


## Configuration
### Windows XP
TLS 1.1 and TLS 1.2 are not supported by Windows XP. Applications which use the OS services for TLS will not be able to use TLS 1.1 or 1.2 on those platforms. However, you can update patches released by Microsoft to support both features. The patch number is KB4019276. Since the patch was released in 2017 (2016, to be exact, but the patch was updated in 2017), Microsoft has a time limit (Windows XP cannot install updates after May 2014), so the patch requires a fix to **update.inf**.


After the installation is complete, you can import the following registry file.
```
Windows Registry Editor Version 5.00 
[HKEY_LOCAL_MACHINE\SYSTEM\WPA\PosReady] 
"Installed"=dword:00000001

Windows Registry Editor Version 5.00
[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\SecurityProviders\SCHANNEL\Protocols\TLS 1.1\Client]
"DisabledByDefault"=dword:00000000

[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\SecurityProviders\SCHANNEL\Protocols\TLS 1.1\Server]
"DisabledByDefault"=dword:00000000


[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\SecurityProviders\SCHANNEL\Protocols\TLS 1.2\Client]
"DisabledByDefault"=dword:00000000

[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\SecurityProviders\SCHANNEL\Protocols\TLS 1.2\Server]
"DisabledByDefault"=dword:00000000
```

### IDE
Take Visual Studio 2019 for example, we can follow these steps:
1. On the **Tools** menu, click **External Tools**.
2. In the **Title** box, type Ubuntu Pastebin Poster.
3. In the **Command** box, type the path to the Ubuntu Pastebin Poster executable, such as D:\Tool\Ubuntu Pastebin Poster.exe
4. In the **Arguments** box, type the path to the file you are interested in, such as $(ItemPath)