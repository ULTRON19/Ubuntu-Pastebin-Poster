# Ubuntu Pastebin Poster
Ubuntu Pastebin Poster is a widget that submits code to <https://paste.ubuntu.com> and displays its return URL. Choose a file and click the **Post** button, wait a moment and you will see the URL returned from the site! 

![Ubuntu Pastebin Poster.png](https://s2.loli.net/2022/01/23/NbOegxdTc39RIiM.png)


## Features
* Drag-and-drop file upload is supported
* Support right-click file upload through the menu
* Available as an external IDE tool
* Automatically copy links to clipboard after posting (v1.0.1+)
* Identify the corresponding programming language based on the file extension (v1.0.1+)
* Arrange drop-down lists based on user preferences (v1.0.1+)
* Support ANSI, UTF-16 LE, UTF-16 BE and UTF-8 encoding (v1.0.2+)

## Requirements
| Name | Options |
|  ----  | ----  |
| **Minimum supported client**  | Windows XP, Windows 2000 Professional with SP3 [desktop apps only] |
| **Minimum supported server**  | Windows Server 2003, Windows 2000 Server with SP3 [desktop apps only] |
| **Target Platform**  | Windows |
| **Redistributable** | WinHTTP 5.1 and Internet Explorer 5.01 or later on Windows XP and Windows 2000. |


## Disclaimer
Do not use this program to automatically send data to the site. Its purpose is simply to make it easy for people to paste. In addition, it is not recommended to use this program to upload large files (≤2M), even if it is theoretically possible.

## Configuration
### setup.ini
After the program is run for the first time, a setup.ini file is generated in the program's directory. It records the last Poster name used (The first run is Windows username), the file extensions corresponding to each programming language, and the user's usage preference. It is generally not recommended to modify this file manually, and if you have to do so, please remember that section and key names cannot be modified. If you want to restore the default Settings, delete this file. The various key-value pairs are described below.
#### name
This key appears only in the Poster section. The key value is a string of up to 30 characters. It records the last Poster name used.
#### suffix
This key appears in all syntax section. The key value is a string. It records the file extensions of the programming language to which the section corresponds. If there are multiple file extensions, link them directly. The data comes from two websites: [HelpOnParsers - MoinMoin](http://moinmo.in/HelpOnParsers) and [languages.json](https://gist.github.com/aymen-mouelhi/82c93fbcd25f091f2c13faa5e0d61760).
#### prefer
This key appears in all section expect Poster. The key value is a non-negative integer. It records the user's usage preference. Sometimes the extension name of the selected file may not correspond to a programming language, or it may correspond to multiple programming languages. In this case, the larger the key value, the higher the section's programming language drop-down list appears.


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