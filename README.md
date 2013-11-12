# hotkeyman
Simple tool to manage your hotkeys under windows.

## General Information
hotkeyman -- short for "*Hotkey-Manager*"  
It uses the Windows-API function `RegisterHotKey()` to register user-defined hotkeys.  
The user is able to define his own hotkeys and their desired effect. More on that in the *Configuration*-section.

## Installation
Simply put a link-file (\*.lnk) to the hotkeyman-binary in your auto-start directory. Usually this directory is located under `C:\Users\[Your_User]\AppData\Roaming\Microsoft\Window s\Start Menu\Programs\Startup`.  
During the next boot of windows *hotkeyman* will automatically start and manage your defined hotkeys.

## Configuration
You will find a configuration-file called `hotkeyman.conf` in the root directory of *hotkeyman*. It already contains an example of how to define your own hotkeys:  
`cmd="start calc.exe" keys="1 2 0 C";`  
Every hotkey-definition contains two key-value pairs.
- **cmd**: The command that will be executed when the hotkey is triggered.
- **keys**: The keys that represent the hotkey
	- Values are delimited with spaces.
    - The first three digits are used to define key-modifiers.
    	- **1** stands for [ALT]
    	- **2** stands for [CTRL]
    	- **4** stands for [SHIFT]
    	- **8** stands for [WINDOWS-KEY]
    	- **0** means that this modifier is not used
        	- For example: Write `keys="0 0 0 C"` defines the hotkey [C].
            - Whereas `keys="1 2 0 C"` defines the hotkey [ALT]+[CTRL]+[C]
    	- For more information take a look at [this](http://msdn.microsoft.com/en-us/library/windows/desktop/ms646309%28v=vs.85%29.aspx).
	- The last character represents the key that is pressed.
  
All together, the example above defines a hotkey 'C' with the modifier-keys [CTRL] and [ALT]. When the hotkey is triggered, the windows-calculator is started.  
Note the semicolon ';' at the end of the example line. It tells the *hotkeyman* that it's the end of the current hotkey definition. This allows you to spread the hotkey-definition over various lines:  
`cmd="start calc.exe"`  
`keys="1 2 0 C";`  
After each semicolon a new hotkey-definition can follow.

## Building *hotkeyman*
Use the existing *Makefile* to build the *hotkeyman*-binary. Type  
`make` or `make debug` to build a debug-version  
`make release` to build a release-version.

### differences
- The debug-build will show a console-window and various debug-messages during the run-time of *hotkeyman*.
- For the relase-build there is only a log-file called `hotkeyman.log` available to view debug-messages. The *hotkeyman*-process runs in the background without any graphical properties.