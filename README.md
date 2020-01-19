# Cortical
## Tobii EyeX plus OpenBCI ganglion for hands-free human interface with computer

This is a personal project and an alpha prototype.  I am actively using this space to experiment with new skills, however the project does work and I am using it for the last couple of years whenever I want to read web pages or emails hands-free.

### Problem : 

I don't want to use the mouse anymore.  I am successfully managing ergonomics, but would love to keep my hands on the keyboard, and use the mouse minimally.

### Solution : 

I purchased a Tobii EyeX and weighed existing software versus custom development.  Eye tracking technology is bleeding edge and existing software was crude and clunky.  Professional solutions are too expensive and not quite what fits me anyway.  I decided to use the SDK to build my own C++ QT desktop application

Eye gaze interactors overlay on screen, providing menus and commands.  The software knows which application has focus and displays context-specific eye-interactors.  EEG data from an OpenBCI Ganglion board has been successfully connected via UDP networking but only displays as a heat map so far.

### Ongoing development : 

This is still an alpha prototype and some configuration is still hardcoded while I experiment, learn QT Framework, and get features functional.  This will receive more polish, perhaps even a rewrite.  Machine learning for EEG triggered mouse-clicks is coming, making the computer potentially fully hands free.
