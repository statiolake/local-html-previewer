##########################################
ローカル HTML ブラウザ (自動リロード付き)
##########################################

自分用に QWebEngineView を使って簡易的にローカルファイルを自動リロードする軽量
ブラウザを作ってみました。自分はこれと pandoc を用いて， Markdown や
reStructuredTextのライブプレビューに利用しています。動作確認は Arch Linux でし
かしていません。

pandoc 自体も inotify でファイルの変更を監視し，変更があったら実行されるように
しています。 pandoc の実行によってその出力の HTML ファイルが書き換えられると，
自動的にこのブラウザがリロードする形です。

だいたい次のようなシェルスクリプト live-preview を作成して使っています。ローカ
ル HTML ブラウザの実行ファイルは local_html_previewer です。

.. code-block:: bash

    #!/bin/bash
    opened=false
    if [[ "$1" == "-n" ]]; then
        # don't open new preview window
        opened=true
        shift 1
    fi
    filename="$1"
    intype="$2"

    [[ -z "$filename" ]] && echo "Usage: $0 Filename [input type]" && exit 1
    [[ -z "$intype" ]] && intype=rst

    while true; do
        pandoc "$filename" -s --self-contained -t html5 -f "$intype+east_asian_line_breaks" -o "preview.html"
        if ! $opened; then
            local_html_previewer "preview.html" &
            opened=true
        fi
        inotifywait -e MODIFY "$filename"
    done
    
