# proxivision-editor
Proxivision Editor - editor that puts together simple but great tools like Fox Toolkit and Scintilla

    cd fox && patch -p1 --dry-run < ../project/fox-1.7-mingw.patch
    cd scintilla && patch -p1 --dry-run < ../project/scintilla-2.28-fxscintilla.patch
    make -j 4 _DEBUG=1 fox
    make -j 4 _DEBUG=1 scintilla
    make -j 4 _DEBUG=1 proxivision
    
    
