# adLSpaceDubEcho

**Stereo dub echo — VST3 · CLAP · Standalone**  
**Version 0.1.18**

[English](#english) · [Français](#français)

## English

Stereo audio delay designed for dub-style echoes and expressive feedback manipulation, with tape-inspired coloration, filtering, modulation and stereo movement. Stereo input/output; no MIDI required.

### Download

Ready-to-use binaries. No compilation required.

| Format | Windows x64 | Linux x86_64 |
|---|---|---|
| VST3 | [ZIP](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Windows_x64_VST3.zip) | [TAR.GZ](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Linux_x86_64_VST3.tar.gz) |
| CLAP | [ZIP](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Windows_x64_CLAP.zip) | [TAR.GZ](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Linux_x86_64_CLAP.tar.gz) |
| Standalone | [ZIP](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Windows_x64_Standalone.zip) | [TAR.GZ](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Linux_x86_64_Standalone.tar.gz) |

[Release and corresponding source code](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/tag/v0.1.18). Linux binaries were built on Ubuntu 24.04; runtime compatibility has not yet been tested.

### Installation

Extract the archive before installation. Keep the entire `.vst3` bundle intact.

| Format | Windows | Linux |
|---|---|---|
| VST3 | `C:\Program Files\Common Files\VST3` | `~/.vst3/` |
| CLAP | `C:\Program Files\Common Files\CLAP` | `~/.clap/` |
| Standalone | Run `adLSpaceDubEcho.exe` | Run `./adLSpaceDubEcho` from a terminal in the extracted directory |

Create the plugin directory if necessary, then rescan plugins in your DAW. Standalone does not require a DAW; select the audio device in its audio settings. On Linux, if needed, restore executable permission with `chmod +x adLSpaceDubEcho`.

### Controls

- **INPUT HI PASS** — Filters low frequencies from the signal sent to the echo (20–2,000 Hz), leaving the dry signal unchanged.
- **MIX** — Balances dry and wet signals.
- **TIME** — Delay time (40–2,000 ms). Changing the time produces tape-like pitch bends.
- **FEEDBACK** — Amount of signal fed back into the delay (0–115%). Above 100%, self-oscillation may occur.
- **TONE** — Reduces high frequencies in the repeats.
- **DRIVE** — Controls echo saturation.
- **TAPE AGING** — Simulates tape aging through progressive high-frequency loss and coloration of repeats.
- **WOW** — Introduces slow pitch fluctuations.
- **FLUTTER** — Introduces rapid pitch fluctuations.
- **TAPE HISS** — Adds tape hiss to the wet signal without feeding it back into the delay.
- **LOW CUT** — High-pass cutoff in the feedback loop (20–5,000 Hz).
- **LOW RESONANCE** — Emphasizes frequencies near the LOW CUT cutoff.
- **HIGH CUT** — Low-pass cutoff in the feedback loop (800–20,000 Hz).
- **HIGH RESONANCE** — Emphasizes frequencies near the HIGH CUT cutoff.
- **OSC CHARACTER** — Shapes feedback-loop saturation, particularly during self-oscillation.
- **MOD DEPTH** — Delay-time modulation depth, up to ±30 ms.
- **MOD RATE** — Modulation rate (0.01–8 Hz).
- **STEREO WIDTH** — Adjusts echo stereo width from mono to an expanded image without increasing feedback gain in normal mode.
- **PING PONG** — Controls cross-feedback between left and right channels, up to alternating repeats.
- **STEREO CHAOS** — Introduces a deliberately unstable interaction between stereo width and cross-feedback.
- **SEND TO ECHO** — Controls the amount of incoming signal sent to the delay. At zero, existing echoes continue to repeat.
- **FEEDBACK BOOST** — Momentary control: hold to increase feedback and fully open the delay send, independently of SEND TO ECHO. On release, the send returns to its normal setting.
- **BOOST LEVEL** — Minimum feedback during boost: 98%, 105% or 115%. A higher existing feedback setting is not reduced.
- **BOOST RELEASE** — Time for feedback to return to its normal setting: 50 ms, 500 ms, 1 s, 3 s, 5 s or 10 s.
- **VU** — Displays the RMS level of the output signal.

High feedback and Stereo Chaos can produce loud signals. Start with a low monitoring level.

### Build from source

CMake 3.22+, C++17, JUCE 9.0.4 and [clap-juce-extensions](https://github.com/free-audio/clap-juce-extensions) (commit `3837f4a6ed2ab0ba46aba69ae5b7041c7c64ce74`, with recursive submodules). Supply local dependency paths:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH="/path/to/JUCE" -DCLAP_JUCE_EXTENSIONS_PATH="/path/to/clap-juce-extensions"
cmake --build build --config Release --target adLSpaceDubEcho_VST3 adLSpaceDubEcho_CLAP adLSpaceDubEcho_Standalone
```

See the [Linux build workflow](.github/workflows/build-linux.yml) for system dependencies and the automated build procedure.

---

## Français

adLSpaceDubEcho est un delay audio stéréo conçu pour les échos dub et la manipulation expressive du feedback. Il associe coloration inspirée de la bande, filtrage, modulation et mouvements stéréo. Entrée et sortie stéréo ; MIDI non requis.

### Téléchargement

Binaires prêts à l’emploi. Aucune compilation nécessaire.

| Format | Windows x64 | Linux x86_64 |
|---|---|---|
| VST3 | [ZIP](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Windows_x64_VST3.zip) | [TAR.GZ](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Linux_x86_64_VST3.tar.gz) |
| CLAP | [ZIP](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Windows_x64_CLAP.zip) | [TAR.GZ](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Linux_x86_64_CLAP.tar.gz) |
| Standalone | [ZIP](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Windows_x64_Standalone.zip) | [TAR.GZ](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/download/v0.1.18/adLSpaceDubEcho_V0.1.18_Linux_x86_64_Standalone.tar.gz) |

[Release et code source correspondant](https://github.com/adLAudioLab/adLSpaceDubEcho/releases/tag/v0.1.18). Les binaires Linux ont été compilés sous Ubuntu 24.04 ; leur fonctionnement réel reste à valider.

### Installation

Extraire l’archive avant installation. Conserver intégralement le dossier `.vst3`.

| Format | Windows | Linux |
|---|---|---|
| VST3 | `C:\Program Files\Common Files\VST3` | `~/.vst3/` |
| CLAP | `C:\Program Files\Common Files\CLAP` | `~/.clap/` |
| Standalone | Lancer `adLSpaceDubEcho.exe` | Lancer `./adLSpaceDubEcho` dans un terminal depuis le dossier extrait |

Créer le dossier de plugins si nécessaire, puis relancer leur détection dans le DAW. L’application autonome ne nécessite aucun DAW ; sélectionner le périphérique dans ses réglages audio. Sous Linux, si nécessaire, rétablir le droit d’exécution avec `chmod +x adLSpaceDubEcho`.

### Commandes

- **INPUT HI PASS** — Filtre les basses du signal envoyé à l’écho (20–2 000 Hz), sans modifier le signal direct.
- **MIX** — Dose le signal direct et le signal traité.
- **TIME** — Durée du delay (40–2 000 ms). Les changements de durée produisent des variations de hauteur comme sur un écho à bande.
- **FEEDBACK** — Quantité de signal réinjectée dans le delay (0–115 %). Au-delà de 100 %, l’auto-oscillation peut apparaître.
- **TONE** — Atténue les aigus des répétitions.
- **DRIVE** — Règle la saturation des échos.
- **TAPE AGING** — Simule le vieillissement de la bande par une perte progressive des aigus et une coloration des répétitions.
- **WOW** — Introduit des fluctuations lentes de hauteur.
- **FLUTTER** — Introduit des fluctuations rapides de hauteur.
- **TAPE HISS** — Ajoute un souffle de bande au signal traité, sans le réinjecter dans le delay.
- **LOW CUT** — Fréquence de coupure du filtre passe-haut dans la boucle de feedback (20–5 000 Hz).
- **LOW RESONANCE** — Accentue les fréquences proches de la coupure du LOW CUT.
- **HIGH CUT** — Fréquence de coupure du filtre passe-bas dans la boucle de feedback (800–20 000 Hz).
- **HIGH RESONANCE** — Accentue les fréquences proches de la coupure du HIGH CUT.
- **OSC CHARACTER** — Modifie le caractère de la saturation dans la boucle de feedback, notamment lors de l’auto-oscillation.
- **MOD DEPTH** — Profondeur de modulation du temps de delay, jusqu’à ±30 ms.
- **MOD RATE** — Vitesse de modulation (0,01–8 Hz).
- **STEREO WIDTH** — Règle la largeur stéréo des échos, du mono à une image élargie, sans augmenter le gain de feedback en mode normal.
- **PING PONG** — Règle l’intensité du feedback croisé entre les canaux gauche et droit, jusqu’à l’alternance des répétitions.
- **STEREO CHAOS** — Introduit une interaction volontairement instable entre la largeur stéréo et le feedback croisé.
- **SEND TO ECHO** — Règle la quantité de signal entrant envoyée au delay. À zéro, les échos déjà présents continuent de se répéter.
- **FEEDBACK BOOST** — Commande momentanée : maintenir pour augmenter le feedback et ouvrir entièrement l’envoi vers le delay, indépendamment de SEND TO ECHO. Au relâchement, l’envoi retrouve son réglage normal.
- **BOOST LEVEL** — Niveau minimal de feedback pendant le boost : 98 %, 105 % ou 115 %. Un feedback déjà supérieur n’est pas réduit.
- **BOOST RELEASE** — Durée de retour du feedback à sa valeur normale : 50 ms, 500 ms, 1 s, 3 s, 5 s ou 10 s.
- **VU** — Indique le niveau RMS du signal de sortie.

Un feedback élevé et Stereo Chaos peuvent produire des niveaux sonores importants. Commencer avec un volume d’écoute faible.

### Compilation depuis les sources

CMake 3.22 minimum, C++17, JUCE 9.0.4 et [clap-juce-extensions](https://github.com/free-audio/clap-juce-extensions) (commit `3837f4a6ed2ab0ba46aba69ae5b7041c7c64ce74`, avec sous-modules récursifs). Indiquer les chemins locaux des dépendances :

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH="/chemin/vers/JUCE" -DCLAP_JUCE_EXTENSIONS_PATH="/chemin/vers/clap-juce-extensions"
cmake --build build --config Release --target adLSpaceDubEcho_VST3 adLSpaceDubEcho_CLAP adLSpaceDubEcho_Standalone
```

Consulter le [workflow Linux](.github/workflows/build-linux.yml) pour les dépendances système et la procédure automatisée.

## License / Licence

adLSpaceDubEcho is licensed under the GNU Affero General Public License v3.0 (AGPLv3). This project uses JUCE, licensed under AGPLv3 or a commercial license. See [LICENSE](LICENSE) for the full license text.

adLSpaceDubEcho est distribué sous licence GNU Affero General Public License v3.0 (AGPLv3). Ce projet utilise JUCE, disponible sous licence AGPLv3 ou sous licence commerciale. Consulter [LICENSE](LICENSE) pour le texte intégral.
