{
    description = "A very basic http server";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    };

    outputs = { self, nixpkgs }: 
        let
            system = "x86_64-linux";
            pkgs = import nixpkgs { inherit system; };
        in 
            {
            devShells.${system}.default = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } rec {
                buildInputs = with pkgs; [
                    cmake
                    valgrind
                ];
                shellHook = ''
                    if [ ! -d build ]; then
                        mkdir build
                        cd build
                        cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
                        cd ..
                        ln -s build/compile_commands.json ./compile_commands.json
                    fi
                '';
            };

            packages.${system}.default = pkgs.clangStdenv.mkDerivation {
                pname = "httpc";
                version = "0.1";
                nativeBuildInputs = with pkgs; [
                    cmake
                ];
                src = ./.;
                buildPhase = ''
                    cmake .
                    make
                '';
                installPhase = ''
                    mkdir -p $out/bin
                    cp httpc $out/bin
                '';
            };
        };
}
