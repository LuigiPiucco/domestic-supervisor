# domestic-supervisor

[![GitHub license](https://img.shields.io/github/license/LuigiPiucco/domestic-supervisor?color=informational)][2] [![Built with nix](https://builtwithnix.org/badge.svg)][1]

Um projeto de brinquedo de simulação de casa automática.

<a href="./README.md">
<img src="https://upload.wikimedia.org/wikipedia/commons/0/0b/English_language.svg" alt="English" title="Read in english" width="32px" />
</a>
<a href="./README.ja.md">
<img src="https://upload.wikimedia.org/wikipedia/en/thumb/9/9e/Flag_of_Japan.svg/1920px-Flag_of_Japan.svg.png" alt="日本語" title="日本語で読みます" width="32px" />
</a>

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->

**Table of Contents**

- [domestic-supervisor](#domestic-supervisor)
  - [Projeto](#projeto)
  - [Design da interface](#design-da-interface)
  - [Instalação](#instalação)
  - [Execução](#execução)
  - [Compilando](#compilando)
  - [Implementação](#implementação)
    - [Entidades comuns](#entidades-comuns)
    - [Entidades do supervisório](#entidades-do-supervisório)
    - [Entidades do simulador](#entidades-do-simulador)
  - [Reconhecimentos](#reconhecimentos)

<!-- markdown-toc end -->

## Projeto

A ideia consiste na criação de dois programas, um que possa servir de
supervisório para os sistemas de uma casa automática e outro que simule os
sistemas da casa, visto que não há bancada de teste disponível.

Eles devem se comunicar pelo protocolo MQTT, amplamente usado para a área de
domótica.

Os sistemas a serem controlados/simulados são:

- Camera com detecção de rostos;
- Travas remotas;
- Luzes remotas;

Funções do supervisório:

- Mostrar a imagem da câmera com uma marcação mostrando onde há (se houver) a
  presença de um rosto;
- Mostrar a planta da casa com indicações das travas e lâmpadas, que podem ser
  clicadas para alternar seu estado;
- Reagir aos eventos enviados pelo simulador, como mudanças de estado de
  lâmpadas ou travas;

Funções do simulador:

- Capturar a imagem da webcam do computador, enviando adequadamente pelo MQTT;
- Reagir aos eventos enviados pelo supervisório, como mudanças de estado de
  lâmpadas ou travas, informando na sua tela e devolvendo o sinal de sucesso
  para o supervisório;
- Simular a alternância das travas ou luzes no local físico, externo ao sistema,
  mostrando a planta da casa e permitindo clicar para alterar o estado;

## Design da interface

Tela da câmera:

![Tela da câmera](./assets/Camera.png)

Tela da planta baixa:

![Tela da planta baixa](./assets/Blueprint.png)

## Instalação

Esses programas não foram feitos para serem instalados a nível de sistema, não é
esperado que alguém os use no seu dia-a-dia. Em vez disso, um pacote tar é
fornecido na seção releases, que contém dois binários e alguns recursos. _A
estrutura de arquivos do pacote deve ser preservada!_

Os binários fornecidos são em sua maioria linkados estaticamente, as únicas
dependências são bibliotecas melhor usadas dinamicamente:

- GraphicsMagick;
- xorg (você provavelmente já tem, omitido no comando);
- libunwind LLVM;
- libc++ e libc++abi LLVM;

Para instalar tudo no Ubuntu, você pode rodar isso:

`# apt install graphicsmagick llvm-11 libc++-11 libc++abi-11`

## Execução

Para rodar os programas, há um pré-requisito. Um Broker MQTT precisar estar
rodando na porta 14442. Qualquer Broker com MQTT5.0 deve servir, mas os
programas são testados apenas com o
[Broker Eclipse Mosquitto](https://mosquitto.org/ "Mosquitto") rodando dentro de
um container do [docker](https://www.docker.com/ "Docker").

Quando o docker estiver instalado e configurado, rode este comando como root
para baixar e rodar a imagem do mosquitto:

`# docker run -p 14442:1883 --name mosquitto -d eclipse-mosquitto `

Para parar o container a qualquer momento, use:

`# docker stop mosquitto`

O container pode parar com reboot dependendo das suas configurações também. Para
iniciá-lo se já estiver instalado, rode em vez do `run` anterior:

`# docker start mosquitto`

Quando o broker estiver pronto e rodando, os programas podem ser iniciados com
esses parâmetros:

`$ /caminho/para/domestic-simulator --docroot ".;/assets" --http-address 0.0.0.0 --http-port 14440 -c /caminho/para/assets/wt_config.xml`

ou

`$ /caminho/para/domestic-supervisor --docroot ".;/assets" --http-address 0.0.0.0 --http-port 14441 -c /path/to/assets/wt_config.xml`

Por fim, abra seu navegador preferido e navegue para `localhost:14440` para o
simulador e `locahost:14441` para o supervisor.

## Compilando

O sistema de compilação por design é o NixOS, usando o `flake.nix` fornecido.
Compilação como um pacote NixOS já é suportada, e suporte para Windows e outras
distros Linux como target está sendo desenvolvido.

Deve ser possível compilar em um ambiente Linux padrão, como Ubuntu ou CentOS,
mas seria necessário especificar manualmente o caminho das dependências para a
CLI do CMake. Se alguém quiser tentar, me avise!

Para compilar, basta clonar o repositório e rodar o seguinte command na pasta
raiz:

`$ nix build '.#domestic-supervisor'`

Note que suporte a flakes precisa estar habilitado para funcionar.

## Implementação

A implementação do projeto foi feita em C++, de acordo com as práticas de bom
código de C++ moderno.

### Entidades comuns

- Namespace `utils`:
  - Múltiplos `event`:
    - Mantém armazenados funções para serem chamadas após ser disparado;
    - Pode ser chamado como função para disparar;
    - Projetado para ser possuído por outro objeto que possui eventos como
      conceito e exposto por referência para que callbacks possam ser
      registrados;
- Namespace `controllers`:
  - 2 `state`:
    - Gerencia o estado da residência, mantendo informações sobre quais lampadas
      e travas estão ligadas;
    - Registra callbacks para realizar ações quando o estado das lampadas e/ou
      travas mudar;
  - 1 `mqtt` que herda de `mqtt::callback` e `mqtt::iaction_listener`:
    - É responsável por processar mensagens de entrada e fornecer
      uma API para enviar mensagens à outra aplicação;
- Múltiplos `device`:
  - Representa um único dispositivo, capaz de ser ativado e desativado;
  - Empacota essa informação de estado com constantes tipo (luz ou trava) e
    nome;
- Interface `view` que herda de `Wt::WContainerWidget`:
  - Representa uma tela de visualização da aplicação;
  - Possui um nome e um caminho além dos elementos de `Wt::WContainerWidget`;
- Entidades que herdam de `view` no namespace `views`;
  - 1 `blueprint`
    - É o widget raiz da tela da planta baixa;
    - É mostrado quando o usuário navega para o caminho "/blueprint";

### Entidades do supervisório

- Namespace `controllers`:
  - 1 `deepnet`:
    - É responsável por aplicar o algoritmo de detecção facial nas imagens
      recebidas e demarcar a posição do rosto na imagem;
    - Processa em uma thread separada, para não bloquear a interface gráfica com o
      processamento de imagem pesado;
    - Permite empurrar imagens em uma fila, as quais serão processadas em ordem
      FIFO;
    - Realiza o processamento por meio de uma rede neural profunda construída
      através da biblioteca dlib;
    - Pode ser desabilitado/habilitado, para diminuir o uso de recursos quando a
      tela da câmera não for a selecionada ou o usuário não queira que sejam
      detectados rostos;
    - Registrar callbacks para realizar ações quando o número de detecções
      encontradas mudar;
- Entidades que herdam de `view` no namespace `views`;
  - 1 `camera`:
    - É o widget raiz da tela da câmera;
    - É mostrado quando o usuário navega para o caminho "/camera" ou "/";
- Entidades que herdam de um descendente de `Wt::WWidget` no namespace `widgets`:
  - 2 `page_anchor`:
    - Desenha o botão de troca de página recebendo os argumentos
      - Posição do canto superior esquerdo;
      - Entidade que implementa `view` a ser mostrada quando se clica no botão;
  - 2 `camera_panel`:
    - Desenha o painel de controle da câmera de acordo com o estado das
      detecções e atualiza a contagem quando esta muda;
- 1 `domestic_supervisor` que herda de `Wt::WApplication`:
  - Representa a aplicação como um todo;
  - Define os comportamentos de tempo de vida da aplicação (startup, connect,
    run, shutdown...);
- 1 `domestic_server` que herda de `Wt::WServer`:
  - Configura o servidor para mandar a aplicação sob multiplas URLs;
  - Cria sessões de `domstic_supervisor` sob demanda. Portanto, na prática, há
    muitas de todas as entidades, mas em nome da simplicidade, apenas o número
    presente numa única sessão é contado nessa lista;

### Entidades do simulador

- Namespace `controllers`:
  - 1 `image_loop`:
    - Coleta e envia as imagens da webcam pelo MQTT;
    - Roda em loop numa thread separada gerenciada por si;
    - É um objeto passível de ser chamado como função;
- 1 `domestic_simulator` que herda de `Wt::WApplication`:
  - Representa a aplicação como um todo;
  - Define os comportamentos de tempo de vida da aplicação (startup, connect,
    run, shutdown...);

## Reconhecimentos

Ícones das luzes feitos por
[Good Ware](https://www.flaticon.com/authors/good-ware "Good Ware") de
https://www.flaticon.com

Ícones dos cadeados feitos por
[CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0 "CC BY-SA 3.0"),
via Wikimedia Commons

Framework Wt feito pela Emweb,
[GPL-2.0-only](https://github.com/emweb/wt/blob/master/LICENSE "Licença do Wt no Github"),
disponível em https://www.webtoolkit.eu/wt

Biblioteca Dlib,
[BSL-1.0](http://dlib.net/license.html "Licença do Dlib"), disponível em
http://dlib.net

Biblioteca OpenCV,
[Apache-2.0](https://github.com/opencv/opencv/blob/master/LICENSE "Licença do OpenCV no Github"),
disponível em https://opencv.org

Biblioteca C++ Paho MQTT feita pela Eclipse Foundation,
[EDL-v1.0](https://github.com/eclipse/paho.mqtt.cpp/blob/master/edl-v10 "Licença do Paho no Github"),
disponível em https://github.com/eclipse/paho.mqtt.cpp

Biblioteca C++ spdlog,
[MIT](https://github.com/gabime/spdlog/blob/v1.x/LICENSE "Licença do spdlog no Github"),
disponível em https://github.com/gabime/spdlog

[1]: https://builtwithnix.org "Construído com nix"
[2]: https://github.com/LuigiPiucco/domestic-supervisor/blob/master/LICENSE "Licença do Github"
