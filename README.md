# まのさば＋

[English](README.md) | [简体中文](README_CN.md)

-----

This project aims to enhance the limited visual presentation of "Magical Girl Witch Trial"

-----

### Build & Run

```powershell

# Generate build files
cmake -B Build

# Build project (Debug configuration by default)
cmake --build Build

# Or build Release configuration (optimized, smaller size)
cmake --build Build --config Release

# Run Debug version
.\Build\Bin\Debug\manosaba_plus.exe

# Run Release version
.\Build\Bin\Release\manosaba_plus.exe

```

## Third-Party Libraries

This project uses the following third-party libraries:

- **[nlohmann/json](https://github.com/nlohmann/json)** (v3.12.0) - JSON for Modern C++ by Niels Lohmann, licensed under the MIT License

See [ThirdParty/nlohmann/README.md](ThirdParty/nlohmann/README.md) for more details.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
