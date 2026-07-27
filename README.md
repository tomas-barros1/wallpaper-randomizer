# Wallpaper Randomizer 🖼️

Um pequeno utilitário em C++ para Windows que troca o papel de parede da área de trabalho automaticamente, sorteando uma imagem aleatória de uma pasta configurada pelo usuário — com a garantia de nunca repetir a última imagem sorteada.

## ✨ Funcionalidades

- Sorteia uma imagem aleatória (`.jpg`, `.jpeg`, `.png`, `.bmp`) de um diretório configurável.
- Nunca repete a última wallpaper sorteada (salva em `lastWallpaper.json`).
- Configuração interativa via terminal na primeira execução — sem precisar editar JSON na mão.
- Troca o papel de parede diretamente via Windows API (`SystemParametersInfoW`).
- Pode ser agendado no Agendador de Tarefas do Windows para trocar o wallpaper automaticamente em intervalos ou no login.

## 📦 Requisitos

- Windows (usa a Windows API, não funciona em Linux/Mac).
- Compilador C++ com suporte a C++17 ou superior (para `<filesystem>`).
- Biblioteca [nlohmann/json](https://github.com/nlohmann/json) (header-only).

## 🔧 Instalação

1. Clone o repositório:
   ```bash
   git clone https://github.com/tomas-barros1/wallpaper-randomizer.git
   cd wallpaper-randomizer
   ```

2. Baixe o header `json.hpp` da [nlohmann/json](https://github.com/nlohmann/json/releases) e coloque em um diretório acessível pelo include path (ex: `include/nlohmann/json.hpp`).

3. Compile o projeto (exemplo com MinGW/g++):
   ```bash
   g++ BackgroundRandomizer/Main.cpp -o wallpaperRandomizer.exe -std=c++17 -luser32 -lgdi32
   ```

   Ou, se estiver usando MSVC (Visual Studio), abra o `BackgroundRandomizer.slnx` e compile normalmente — as bibliotecas `user32.lib` e `gdi32.lib` já vêm linkadas por padrão em projetos Windows.

## 🚀 Uso

1. Rode o executável pela primeira vez:
   ```bash
   wallpaperRandomizer.exe
   ```

2. Na primeira execução, o programa vai perguntar o caminho da pasta com as imagens:
   ```
   Nenhum diretorio de wallpapers configurado ainda.
   Digite o caminho completo da pasta com as imagens (ex: D:\Wallpapers ou D:/Wallpapers):
   ```

3. Esse caminho é salvo automaticamente em `wallpapersDir.json`, já normalizado. Nas próximas execuções, o programa lê esse arquivo direto — não precisa digitar de novo.

4. A cada execução, uma imagem aleatória (diferente da última sorteada) é definida como papel de parede, e o caminho fica salvo em `lastWallpaper.json`.

### Reconfigurar o diretório

Se quiser trocar a pasta de wallpapers, apague o arquivo `wallpapersDir.json` e rode o programa novamente — ele vai perguntar o caminho de novo.

## 📁 Arquivos gerados

| Arquivo | Descrição |
|---|---|
| `wallpapersDir.json` | Guarda o caminho da pasta com as imagens. |
| `lastWallpaper.json` | Guarda o caminho da última imagem sorteada, usado para evitar repetição. |

Exemplo de `wallpapersDir.json`:
```json
{
    "wallpapersDir": "D:/Wallpapers"
}
```

## ⏰ Agendando execução automática (opcional)

Para trocar o wallpaper automaticamente em intervalos (ex: a cada hora) ou no login:

1. Abra o **Agendador de Tarefas** do Windows.
2. Crie uma nova tarefa com o gatilho desejado (ex: "Ao fazer logon" ou "Diariamente a cada X horas").
3. Na aba **Ações**, aponte para o `.exe` e **preencha o campo "Iniciar em (opcional)"** com a pasta onde está o executável — isso é essencial, pois o programa usa caminhos relativos para os arquivos `.json`.
4. Na aba **Geral**, marque **"Executar somente quando o usuário estiver conectado"** (a troca de wallpaper não funciona em sessões sem desktop).

> 💡 Dica: se quiser depurar problemas de execução via Agendador, crie um `.bat` intermediário que redireciona a saída para um log:
> ```bat
> @echo off
> cd /d D:\Caminho\Para\O\Projeto
> wallpaperRandomizer.exe > log.txt 2>&1
> ```

## 🛠️ Possíveis melhorias futuras

- Suporte a subpastas (busca recursiva de imagens).
- Suporte a mais formatos de imagem (`.webp`, `.gif`).
- Interface gráfica simples para configuração.
- Histórico de wallpapers já usados (evitar repetição em uma janela maior, não só a última).
