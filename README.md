# adLSpaceDubEcho

**Stereo dub echo audio effect — VST3 and standalone application**  
**Version:** 0.1.0 (CMake project version)

[English](#english) · [Français](#français)

## English

### Overview

adLSpaceDubEcho is a stereo audio delay designed for dub-style echoes and expressive feedback manipulation. It combines delay feedback, tone shaping, modulation, stereo movement and tape-inspired coloration.

### Features

- Stereo delay with adjustable **TIME** (40–2000 ms), **FEEDBACK**, and **DRY / WET** mix.
- **SEND TO ECHO** control to adjust how much incoming audio feeds the delay.
- Momentary **FEEDBACK BOOST**: hold to raise feedback and inject incoming audio into the delay, independently of the normal send level.
- Three boost targets: **98%**, **105%**, **115%**; six release times: **50 ms**, **500 ms**, **1 s**, **3 s**, **5 s**, **10 s**.
- **TONE**, **DRIVE**, **MOD DEPTH**, **MOD RATE**, **WIDTH**, and **PING PONG** controls.
- Tape-inspired processing: **TAPE AGING**, **WOW**, **FLUTTER**, and **TAPE HISS**.
- Additional shaping: **LOW CUT**, **HIGH CUT**, **LOW RES**, **HIGH RES**, **INPUT HPF**, **OSC CHARACTER**, and **STEREO CHAOS**.
- Custom JUCE graphical interface with artwork embedded in source headers.

### Formats and requirements

- **Formats configured:** VST3 and Standalone.
- **Audio:** stereo input and stereo output.
- **MIDI:** not required.
- **Build system:** CMake 3.22 or later; C++17; JUCE.
- **Build environment:** the current project configuration uses a Windows-specific JUCE source path. Other environments have not been verified.

### Build (current Windows setup)

The current `CMakeLists.txt` expects JUCE at `C:/AudioDev/JUCE`. Install JUCE there, or edit the path in `CMakeLists.txt` before configuring. From the project directory:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

These commands describe the configured build process; they are not a claim that every compiler or host has been tested.

### Project status

Source code is available in this repository. A downloadable binary release and a formal open-source license have not yet been added. Until a license is published, no permission to reuse or redistribute the source code should be assumed.

---

## Français

### Présentation

adLSpaceDubEcho est un delay audio stéréo conçu pour les échos dub et la manipulation expressive du feedback. Il associe répétitions, filtrage, modulation, mouvements stéréo et coloration inspirée des échos à bande.

### Fonctionnalités

- Delay stéréo avec **TIME** (40 à 2 000 ms), **FEEDBACK** et mixage **DRY / WET**.
- **SEND TO ECHO** pour régler la quantité de signal entrant envoyée vers le delay.
- **FEEDBACK BOOST** momentané : maintenir la commande pour augmenter le feedback et injecter le signal entrant, indépendamment du niveau d'envoi normal.
- Trois niveaux de boost : **98 %**, **105 %**, **115 %** ; six durées de relâchement : **50 ms**, **500 ms**, **1 s**, **3 s**, **5 s**, **10 s**.
- Réglages **TONE**, **DRIVE**, **MOD DEPTH**, **MOD RATE**, **WIDTH** et **PING PONG**.
- Traitements inspirés de la bande : **TAPE AGING**, **WOW**, **FLUTTER** et **TAPE HISS**.
- Réglages supplémentaires : **LOW CUT**, **HIGH CUT**, **LOW RES**, **HIGH RES**, **INPUT HPF**, **OSC CHARACTER** et **STEREO CHAOS**.
- Interface JUCE personnalisée, avec ressources graphiques intégrées aux fichiers d'en-tête.

### Formats et prérequis

- **Formats configurés :** VST3 et application autonome (Standalone).
- **Audio :** entrée et sortie stéréo.
- **MIDI :** non requis.
- **Compilation :** CMake 3.22 minimum, C++17 et JUCE.
- **Environnement :** la configuration actuelle utilise un chemin JUCE propre à Windows. Les autres environnements ne sont pas encore vérifiés.

### Compilation (configuration Windows actuelle)

Le fichier `CMakeLists.txt` recherche JUCE dans `C:/AudioDev/JUCE`. Installer JUCE à cet emplacement ou modifier ce chemin dans `CMakeLists.txt`, puis exécuter depuis le dossier du projet :

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Ces commandes décrivent la procédure prévue par la configuration actuelle ; elles ne constituent pas une validation sur tous les compilateurs ou logiciels hôtes.

### État du projet

Le code source est disponible dans ce dépôt. Aucune version binaire téléchargeable ni licence open source définitive n'a encore été ajoutée. En l'absence de licence publiée, il ne faut pas présumer d'une autorisation de réutilisation ou de redistribution du code.

## License / Licence

### English

adLSpaceDubEcho is licensed under the GNU Affero General Public License v3.0 (AGPLv3).

This project uses JUCE, licensed under AGPLv3 or a commercial license.

See [LICENSE](LICENSE) for the full license text.

### Français

adLSpaceDubEcho est distribué sous licence GNU Affero General Public License v3.0 (AGPLv3).

Ce projet utilise JUCE, disponible sous licence AGPLv3 ou sous licence commerciale.

Consultez le fichier [LICENSE](LICENSE) pour le texte intégral de la licence.
