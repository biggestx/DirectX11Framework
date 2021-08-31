cd..
cd..

xcopy .\Engine\Include\*.h  .\BiggestEngine\Include\Engine\ /y /d /s /exclude:Out.txt
xcopy .\Engine\Include\*.inl .\BiggestEngine\Include\Engine\ /y /d /s /exclude:Out.txt
xcopy .\Engine\Bin\*.* .\BiggestEngine\Bin\Engine\ /y /d /s /exclude:Out.txt
xcopy .\Engine\Bin\*.* .\Client\Bin\ /y /d /s /exclude:Out.txt