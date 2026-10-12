# QMK Userspace

This is a template repository which allows for an external set of QMK keymaps to be defined and compiled. This is useful for users who want to maintain their own keymaps without having to fork the [main QMK repository](https://github.com/qmk/qmk_firmware). You must still fork the main QMK repository if writing firmware for a *new* keyboard.

## Howto configure your build targets

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. Enable userspace in QMK config using `qmk config user.overlay_dir="$(realpath qmk_userspace)"`
1. Add a new keymap for your board using `qmk new-keymap`
    * This will create a new keymap in the `keyboards` directory, in the same location that would normally be used in the main QMK repository. For example, if you wanted to add a keymap for the Planck, it will be created in `keyboards/planck/keymaps/<your keymap name>`
    * You can also create a new keymap using `qmk new-keymap -kb <your_keyboard> -km <your_keymap>`
    * Alternatively, add your keymap manually by placing it in the location specified above.
    * `layouts/<layout name>/<your keymap name>/keymap.*` is also supported if you prefer the layout system
1. Add your keymap(s) to the build by running `qmk userspace-add -kb <your_keyboard> -km <your_keymap>`
    * This will automatically update your `qmk.json` file
    * Corresponding `qmk userspace-remove -kb <your_keyboard> -km <your_keymap>` will delete it
    * Listing the build targets can be done with `qmk userspace-list`
1. Commit your changes

## Howto build with GitHub

1. In the GitHub Actions tab, enable workflows
1. Push your changes above to your forked GitHub repository
1. Look at the GitHub Actions for a new actions run
1. Wait for the actions run to complete
1. Inspect the Releases tab on your repository for the latest firmware build

## Howto build locally

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. `cd` into this repository's clone directory
1. Set global userspace path: `qmk config user.overlay_dir="$(realpath .)"` -- you MUST be located in the cloned userspace location for this to work correctly
    * This will be automatically detected if you've `cd`ed into your userspace repository, but the above makes your userspace available regardless of your shell location.
1. Compile normally: `qmk compile -kb your_keyboard -km your_keymap` or `make your_keyboard:your_keymap`

Alternatively, if you configured your build targets above, you can use `qmk userspace-compile` to build all of your userspace targets at once.

## EC Type-K keymap diagram

The [printable satt keymap](docs/keymaps/ec-typek-satt-a4.pdf) contains all three
layers on one A4 portrait page. Print at 100% / actual size. Yellow keys activate
layers, blue keys change on the selected layer, and gray keys inherit the base
layer. The diagram shows each layer separately; when both upper layers are active,
transparent keys fall through from layer 2 to layer 1 to the base layer.

### Regenerate the PDF

Requirements: Python 3.10 or newer, `rsvg-convert` (from librsvg), and a Japanese
font such as macOS's Hiragino Sans or Noto Sans CJK JP. On macOS:

```sh
brew install python librsvg
```

From this repository's root, run:

```sh
python3 scripts/render_keymap.py
```

The script reads `keyboards/cipulot/ec_typek/keymaps/satt/keymap.c` from this
repository and physical key coordinates from
`../qmk_firmware/keyboards/cipulot/ec_typek/keyboard.json`. If your QMK firmware
checkout is elsewhere, specify its directory:

```sh
python3 scripts/render_keymap.py --qmk-dir /path/to/qmk_firmware
```

The PDF is written to `docs/keymaps/ec-typek-satt-a4.pdf`. SVGs, PNG previews, and
parsed keymap JSON are also generated there but ignored by Git. Use `--no-png`
to skip PNG previews while still generating the PDF, or `--output-dir` to change
the output directory. Without `rsvg-convert`, only SVGs and JSON are generated.
The script uses only the Python standard library and checks layer/key counts and
display labels before drawing. If you add an unfamiliar keycode or change a
custom key's behavior, update the script's display labels and explanatory notes
as needed, regenerate, and commit the updated PDF with the source changes.

## Extra info

If you wish to point GitHub actions to a different repository, a different branch, or even a different keymap name, you can modify `.github/workflows/build_binaries.yml` to suit your needs.

To override the `build` job, you can change the following parameters to use a different QMK repository or branch:
```
    with:
      qmk_repo: qmk/qmk_firmware
      qmk_ref: master
```

If you wish to manually manage `qmk_firmware` using git within the userspace repository, you can add `qmk_firmware` as a submodule in the userspace directory instead. GitHub Actions will automatically use the submodule at the pinned revision if it exists, otherwise it will use the default latest revision of `qmk_firmware` from the main repository.

This can also be used to control which fork is used, though only upstream `qmk_firmware` will have support for external userspace until other manufacturers update their forks.

1. (First time only) `git submodule add https://github.com/qmk/qmk_firmware.git`
1. (To update) `git submodule update --init --recursive`
1. Commit your changes to your userspace repository
