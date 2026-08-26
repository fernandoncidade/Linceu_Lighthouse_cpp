#include "DOC_PT_BR.h"

namespace source::ui::manual {

std::vector<ManualSection> doc_pt_br() {
    const QString dataDir = data_dir();
    return {
        {
            QStringLiteral("novos-recursos-funcionalidades"),
            QStringLiteral("Novos recursos e funcionalidades"),
            {
                QStringLiteral("Foram implementadas melhorias de confiabilidade no monitoramento em tempo real, no controle de pausa/retomada/parada e na atualização da tabela em cenários com alto volume de eventos."),
                QStringLiteral("As instruções abaixo ajudam a usar os novos comportamentos de forma segura e previsível durante a operação diária."),
            },
            {
                QStringLiteral("Detecção mais precisa para adição, modificação, exclusão, renomeação e movimentação de arquivos."),
                QStringLiteral("Atualização visual mais estável em listas grandes, com menor risco de inconsistência durante picos de atividade."),
                QStringLiteral("Troca de idioma mais consistente em menus, tabela, manual, ajuda e estatísticas, sem exigir reinicialização."),
            },
            {
                {
                    QStringLiteral("Fluxo recomendado para monitoramento contínuo"),
                    {
                        QStringLiteral("1) Adicione os diretórios desejados e execute um scan inicial completo."),
                        QStringLiteral("2) Confirme o preenchimento da tabela e o status da interface."),
                        QStringLiteral("3) Ative o monitoramento contínuo para capturar eventos novos em tempo real."),
                        QStringLiteral("4) Use filtros para reduzir ruído e acelerar auditorias."),
                    },
                    {}
                },
                {
                    QStringLiteral("Como usar pausa, retomada e parada com segurança"),
                    {
                        QStringLiteral("Durante operações extensas, pause o monitoramento para revisão intermediária de resultados."),
                        QStringLiteral("Retome somente após validar os dados exibidos, mantendo controle sobre a janela de análise."),
                        QStringLiteral("Para encerrar, interrompa monitoramento e escaneamento de forma explícita e aguarde a atualização final de status."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("abrir-arquivos-mime-datas-pastas"),
            QStringLiteral("Abertura de arquivos, MIME e datas de pastas"),
            {
                QStringLiteral("A tabela agora ajuda a abrir os itens reportados mesmo quando eles não são arquivos de texto. Ao usar Abrir ou duplo clique em uma linha de arquivo, o aplicativo tenta usar o aplicativo padrão do Windows e pode oferecer outras opções compatíveis."),
                QStringLiteral("A coluna MIME exibe o tipo real de conteúdo em linguagem legível no idioma atual, mantendo o identificador original entre parênteses para auditoria. As colunas Data de Criação, Data de Modificação e Data de Acesso também são preenchidas para pastas, subpastas e diretórios filhos durante a varredura."),
            },
            {
                QStringLiteral("Use Abrir ou duplo clique para abrir documentos, imagens, vídeos, planilhas, executáveis, arquivos compactados e outros tipos reconhecidos pelo sistema."),
                QStringLiteral("Quando o Windows informar mais de uma opção, escolha o aplicativo desejado na janela de seleção."),
                QStringLiteral("Ative a coluna MIME para conferir a categoria e o subtipo de conteúdo detectado."),
                QStringLiteral("Ative as três colunas de data para auditar quando pastas e arquivos foram criados, modificados ou acessados."),
            },
            {
                {
                    QStringLiteral("Como abrir qualquer tipo de arquivo"),
                    {
                        QStringLiteral("1) Selecione uma linha que represente um arquivo existente."),
                        QStringLiteral("2) Dê duplo clique ou use a ação Abrir."),
                        QStringLiteral("3) Se aparecer uma janela de escolha, selecione o aplicativo desejado e confirme."),
                        QStringLiteral("4) Se o item for uma pasta, use a ação de abrir pasta para navegar até o local no Explorador de Arquivos."),
                    },
                    {}
                },
                {
                    QStringLiteral("Como interpretar a coluna MIME"),
                    {
                        QStringLiteral("A primeira parte mostra a família do conteúdo, como Texto, Imagem, Vídeo, Áudio ou Aplicativo."),
                        QStringLiteral("A segunda parte mostra o subtipo em linguagem simples, como PDF, PNG, ZIP ou Texto simples."),
                        QStringLiteral("O texto entre parênteses preserva o identificador original, por exemplo text/plain ou application/pdf."),
                    },
                    {}
                },
                {
                    QStringLiteral("Como conferir datas em pastas e subpastas"),
                    {
                        QStringLiteral("1) Abra o menu de colunas e habilite Data de Criação, Data de Modificação e Data de Acesso."),
                        QStringLiteral("2) Execute uma nova varredura no diretório escolhido."),
                        QStringLiteral("3) Ordene ou filtre a tabela para revisar pastas filhas e diretórios mais profundos."),
                        QStringLiteral("4) Use essas datas para comparar alterações recentes com eventos registrados."),
                    },
                    {}
                },
                {
                    QStringLiteral("Se um arquivo não abrir"),
                    {
                        QStringLiteral("Confirme se o caminho ainda existe e se você possui permissão de leitura."),
                        QStringLiteral("Verifique se há um aplicativo associado ao tipo de arquivo no Windows."),
                        QStringLiteral("Quando necessário, escolha manualmente outro aplicativo compatível na janela de seleção."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("operacao-em-grandes-volumes"),
            QStringLiteral("Operação em grandes volumes"),
            {
                QStringLiteral("Quando há muitos arquivos e eventos em sequência, a recomendação é operar por ciclos curtos: capturar, revisar, filtrar e exportar."),
            },
            {
                QStringLiteral("Ordene por data/evento para priorizar alterações mais recentes."),
                QStringLiteral("Aplique filtros por tipo e diretório antes de executar ações de exportação."),
                QStringLiteral("Use pausa estratégica para evitar revisão em fluxo contínuo sem ponto de controle."),
            },
            {
                {
                    QStringLiteral("Troca de idioma durante uso intenso"),
                    {
                        QStringLiteral("A troca de idioma pode ser feita com o monitoramento ativo, mas é recomendável pausar durante validações visuais críticas."),
                        QStringLiteral("Após alterar o idioma, confirme textos em menu, cabeçalhos da tabela, ajuda e estatísticas."),
                    },
                    {}
                },
                {
                    QStringLiteral("Validação rápida de estabilidade"),
                    {
                        QStringLiteral("Confira se eventos recentes continuam aparecendo após retomar o monitoramento."),
                        QStringLiteral("Verifique se os rótulos de status e barra de progresso mudam corretamente entre estados."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("recursos-complementares-uso"),
            QStringLiteral("Recursos complementares de uso"),
            {
                QStringLiteral("Esta versão inclui recursos adicionais voltados à operação diária: popup de segurança configurável, classificação refinada de eventos e abertura de estatísticas com validação de consistência."),
            },
            {
                QStringLiteral("Popup de segurança com controle de limite mínimo, máximo de linhas e intervalo entre alertas."),
                QStringLiteral("Separação mais clara entre eventos Renomeado e Movido para auditoria."),
                QStringLiteral("Fluxo de estatísticas com maior consistência quando há gravações pendentes."),
            },
            {
                {
                    QStringLiteral("Como configurar o popup de segurança"),
                    {
                        QStringLiteral("Abra o menu Segurança e acesse a configuração de popup/notificações."),
                        QStringLiteral("Defina o limite mínimo para disparo do alerta."),
                        QStringLiteral("Ajuste o máximo de linhas por popup para controlar a densidade visual."),
                        QStringLiteral("Configure o intervalo entre alertas e salve as preferências."),
                    },
                    {}
                },
                {
                    QStringLiteral("Como interpretar Renomeado x Movido"),
                    {
                        QStringLiteral("Renomeado: mesmo diretório, alteração apenas no nome."),
                        QStringLiteral("Movido: mudança de diretório, com ou sem mudança de nome."),
                        QStringLiteral("Em revisões, filtre por Evento e confira origem/destino no painel de detalhes."),
                    },
                    {}
                },
                {
                    QStringLiteral("Como abrir estatísticas com dados estáveis"),
                    {
                        QStringLiteral("Conclua operações pesadas ou use pausa antes de abrir a janela de estatísticas."),
                        QStringLiteral("Ao abrir, valide se não há pendências de processamento no status."),
                        QStringLiteral("Se houver divergência pontual, feche e abra novamente após estabilização."),
                    },
                    {}
                },
                {
                    QStringLiteral("Modo gráfico avançado (opcional)"),
                    {
                        QStringLiteral("Em cenários de incompatibilidade gráfica, defina a variável LINCEU_OPENGL_MODE antes de iniciar o aplicativo."),
                        QStringLiteral("Valores aceitos: auto, desktop, angle e software."),
                        QStringLiteral("Use software apenas para diagnóstico, pois pode reduzir desempenho."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("fluxo-operacional-menus-atualizados"),
            QStringLiteral("Fluxo operacional dos menus atualizados"),
            {
                QStringLiteral("Os menus Arquivo, Configurações e Opções foram ampliados para cobrir o ciclo completo de trabalho: escolher diretórios, iniciar ou pausar análise, preparar a tabela, configurar estatísticas, exportar dados e consultar recursos de segurança."),
            },
            {
                QStringLiteral("Use Arquivo > Selecionar Diretório ou Ctrl+Shift+O para escolher a pasta principal antes do scan."),
                QStringLiteral("Use Arquivo > Iniciar/Parar ou Ctrl+R para iniciar ou interromper a análise, e Arquivo > Pausar ou Ctrl+P para criar um ponto de revisão sem perder o estado atual."),
                QStringLiteral("Use Configurações > Filtros, Colunas e Colunas coloridas antes de exportar ou abrir estatísticas, porque esses ajustes ajudam a reduzir ruído e destacar campos importantes."),
                QStringLiteral("Use Opções > Manual sempre que precisar reabrir estas instruções durante a operação, sem reiniciar o aplicativo."),
            },
            {
                {
                    QStringLiteral("Sequência recomendada"),
                    {
                        QStringLiteral("1. Selecione um diretório e execute o scan inicial."),
                        QStringLiteral("2. Ajuste filtros, colunas e cores para deixar visíveis apenas os dados necessários."),
                        QStringLiteral("3. Abra Configurações > Estatísticas > Plotar Gráficos e escolha os gráficos úteis para a auditoria."),
                        QStringLiteral("4. Abra Arquivo > Estatísticas ou pressione Ctrl+G para gerar a análise visual."),
                        QStringLiteral("5. Exporte resultados somente depois de confirmar que a tabela e os gráficos mostram o recorte correto."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-menu-estatisticas-plotar-graficos"),
            QStringLiteral("Como usar Configurações > Estatísticas > Plotar Gráficos"),
            {
                QStringLiteral("Esse submenu controla quais gráficos serão gerados na janela de estatísticas. Os itens marcados são salvos em statistics_charts_config.json no diretório persistente do aplicativo e passam a ser usados nas próximas aberturas da janela."),
            },
            {
                QStringLiteral("Marque apenas os gráficos que deseja gerar para reduzir tempo de processamento e deixar a janela mais objetiva."),
                QStringLiteral("Desmarque gráficos pesados ou desnecessários quando estiver analisando muitos eventos."),
                QStringLiteral("Depois de alterar a seleção, abra Arquivo > Estatísticas ou use Ctrl+G para gerar a janela com o conjunto escolhido."),
            },
            {
                {
                    QStringLiteral("Quando usar cada grupo de gráfico"),
                    {
                        QStringLiteral("Use Pizza, Barras, Pareto e Distribuições para comparar volumes por tipo, tamanho ou operação."),
                        QStringLiteral("Use Timeline, Operações por dia, Semana/Hora e Heatmap para investigar concentração temporal de eventos."),
                        QStringLiteral("Use Sankey, Evento por Diretório e Tipo por Diretório para descobrir fluxos entre operação, tipo de arquivo e pasta monitorada."),
                        QStringLiteral("Use Árvore de Diretórios para localizar quais níveis de pasta concentram mais atividade."),
                        QStringLiteral("Use Scatter, Radar e Dotplot quando precisar comparar padrões de tamanho, frequência e distribuição de operações."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-janela-estatisticas"),
            QStringLiteral("Como operar a janela de Estatísticas"),
            {
                QStringLiteral("A janela de estatísticas lê o banco de monitoramento e apresenta os gráficos selecionados. Abra-a somente depois de existir dados na tabela; em scans muito pesados, aguarde o status estabilizar antes de atualizar os gráficos."),
            },
            {
                QStringLiteral("Use o painel Selecionar gráficos para mostrar ou ocultar gráficos na sessão atual sem alterar necessariamente a análise já carregada."),
                QStringLiteral("Use Selecionar todos para reativar todos os gráficos disponíveis na janela."),
                QStringLiteral("Use Salvar selecionados para exportar somente os gráficos marcados em PNG."),
                QStringLiteral("Use Salvar todos para exportar todos os gráficos gerados para uma pasta com data e hora no nome."),
                QStringLiteral("Use Atualizar depois de novos eventos, após mudar filtros relevantes ou após alterar a seleção no menu de gráficos."),
                QStringLiteral("Oculte o painel de seleção quando precisar de mais espaço para comparar abas e detalhes visuais."),
            },
            {}
        },
        {
            QStringLiteral("uso-menu-seguranca"),
            QStringLiteral("Como usar Configurações > Segurança"),
            {
                QStringLiteral("O submenu Segurança reúne recursos de auditoria, acompanhamento de sessões, quarentena, restauração e pacote de evidência. Use-o depois de configurar diretórios e, preferencialmente, após um scan inicial."),
            },
            {},
            {
                {
                    QStringLiteral("Perfis e multi-diretório"),
                    {
                        QStringLiteral("Abra Configurações > Segurança > Perfis e multi-diretório para gerenciar o perfil ativo."),
                        QStringLiteral("Use Adicionar diretório para incluir outra pasta no mesmo perfil e Remover selecionado para retirar a pasta destacada."),
                        QStringLiteral("Edite Nome do perfil e clique Salvar perfil para identificar melhor o conjunto de pastas monitoradas."),
                        QStringLiteral("Clique Atualizar para recarregar o resumo de diretório principal, total de diretórios e perfil atual."),
                    },
                    {}
                },
                {
                    QStringLiteral("Central de alertas e Sessões em tempo real"),
                    {
                        QStringLiteral("Abra Central de alertas para revisar horário, nível de risco, resumo, regras acionadas, caminho, nome, sessão e evidência."),
                        QStringLiteral("Priorize alertas críticos ou altos antes de revisar eventos comuns na tabela principal."),
                        QStringLiteral("Abra Sessões em tempo real para acompanhar sessões recentes, total de eventos e diretórios envolvidos."),
                        QStringLiteral("Use Atualizar em cada janela enquanto o monitoramento estiver ativo para carregar registros novos sem fechar o diálogo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Quarentena, restauração e evidências"),
                    {
                        QStringLiteral("Abra Quarentena e restauração para consultar as abas Quarentena e Versões monitoradas."),
                        QStringLiteral("Na aba Quarentena, selecione uma linha e use Abrir pasta selecionada para localizar a cópia isolada; use Restaurar cópia selecionada somente após conferir o destino."),
                        QStringLiteral("Na aba Versões monitoradas, selecione uma linha e use Restaurar versão monitorada para recuperar uma versão acompanhada pelo sistema."),
                        QStringLiteral("Abra Pacote de evidência, escolha o destino, clique Exportar agora e depois abra a pasta gerada para conferir os arquivos exportados."),
                        QStringLiteral("Use Limpar banco de segurança apenas depois de exportar evidências ou quando tiver certeza de que o histórico antigo não será mais necessário."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-notificacoes-exportacao"),
            QStringLiteral("Como usar notificações e opções de exportação"),
            {
                QStringLiteral("As notificações de segurança e as opções de exportação ajudam a controlar volume de informação, frequência de avisos e formato dos dados salvos."),
            },
            {
                QStringLiteral("Abra Configurações > Segurança > Notificações para ativar resumos de segurança no rodapé/barra de status ou configurar pop-ups por tipo."),
                QStringLiteral("Em Pop-ups de alertas, sessões ou quarentena, use Ativar este pop-up para ligar/desligar o aviso, Disparar ao atingir para definir o limite mínimo, Máximo de linhas no resumo para controlar o tamanho e Cooldown entre pop-ups para evitar avisos repetidos."),
                QStringLiteral("Abra Configurações > Opções de exportação para decidir se o arquivo salvo deve respeitar colunas ativas, filtros ativos ou somente os dados selecionados."),
                QStringLiteral("Use Redefinir opções de exportação quando quiser voltar ao comportamento padrão antes de gerar CSV, TXT, JSON, XML, DB, XLSX ou pacote de evidência."),
            },
            {}
        },
        {
            QStringLiteral("visao-geral"),
            QStringLiteral("Visão Geral"),
            {
            QStringLiteral("Linceu Lighthouse permite inspecionar, monitorar e analisar arquivos e pastas em múltiplas localizações do seu computador. Apresenta uma lista central com informações e ações para organizar, filtrar, gerar estatísticas e extrair metadados."),
        },
            {},
            {}
        },
        {
            QStringLiteral("tela-principal"),
            QStringLiteral("Tela Principal — Elementos"),
            {},
            {
            QStringLiteral("𝐁𝐚𝐫𝐫𝐚 𝐝𝐞 𝐟𝐞𝐫𝐫𝐚𝐦𝐞𝐧𝐭𝐚𝐬: Acesso rápido a: Adicionar Diretório, Iniciar Scan, Parar, Atualizar e Preferências."),
            QStringLiteral("𝐒𝐞𝐥𝐞çã𝐨 𝐝𝐞 𝐝𝐢𝐫𝐞𝐭ó𝐫𝐢𝐨𝐬: Área para adicionar, remover ou alternar pastas monitoradas."),
            QStringLiteral("𝐓𝐚𝐛𝐞𝐥𝐚 𝐝𝐞 𝐢𝐭𝐞𝐧𝐬: Lista central com colunas (Nome, Tipo, Tamanho, Data, Evento) e suporte a ordenação/filtragem."),
            QStringLiteral("𝐏𝐚𝐢𝐧𝐞𝐥 𝐝𝐞 𝐝𝐞𝐭𝐚𝐥𝐡𝐞𝐬: Exibe metadados e ações rápidas para o item selecionado."),
            QStringLiteral("𝐌𝐞𝐧𝐬𝐚𝐠𝐞𝐧𝐬 𝐞 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐬𝐨: Mostra notificações, logs de operação e barras de progresso."),
            QStringLiteral("𝐄𝐬𝐭𝐚𝐭í𝐬𝐭𝐢𝐜𝐚𝐬: Área para gráficos e relatórios gerados a partir dos dados coletados."),
        },
            {}
        },
        {
            QStringLiteral("como-comecar"),
            QStringLiteral("Como Começar"),
            {},
            {
            QStringLiteral("Abra o aplicativo pelo atalho ou executável."),
            QStringLiteral("Clique em 𝐀𝐝𝐢𝐜𝐢𝐨𝐧𝐚𝐫 𝐃𝐢𝐫𝐞𝐭ó𝐫𝐢𝐨 e escolha a pasta a ser monitorada."),
            QStringLiteral("Execute um scan inicial com 𝐈𝐧𝐢𝐜𝐢𝐚𝐫/𝐒𝐜𝐚𝐧."),
            QStringLiteral("Aguarde; os itens serão listados com os metadados disponíveis."),
            QStringLiteral("Ative 𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐚𝐦𝐞𝐧𝐭𝐨 𝐜𝐨𝐧𝐭í𝐧𝐮𝐨 para atualizações automáticas."),
        },
            {}
        },
        {
            QStringLiteral("modos-operacao"),
            QStringLiteral("Modos de Operação"),
            {},
            {
            QStringLiteral("𝐒𝐜𝐚𝐧 Ú𝐧𝐢𝐜𝐨: Verificação imediata da pasta selecionada."),
            QStringLiteral("𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐚𝐦𝐞𝐧𝐭𝐨 𝐜𝐨𝐧𝐭í𝐧𝐮𝐨: Observação em tempo real de alterações e atualização automática da tabela."),
            QStringLiteral("𝐏𝐫𝐨𝐜𝐞𝐬𝐬𝐚𝐦𝐞𝐧𝐭𝐨 𝐞𝐦 𝐥𝐨𝐭𝐞: Extração de metadados ou ações para múltiplos arquivos simultaneamente."),
        },
            {}
        },
        {
            QStringLiteral("entendendo-tabela"),
            QStringLiteral("Entendendo a Tabela de Itens"),
            {
            QStringLiteral("Cada coluna representa uma propriedade do item. Use ordenação, filtragem e seleção para localizar e analisar itens rapidamente."),
        },
            {
            QStringLiteral("𝐍𝐨𝐦𝐞: Nome do arquivo ou pasta; duplo clique abre o item."),
            QStringLiteral("𝐓𝐢𝐩𝐨: Tipo detectado (imagem, vídeo, documento, pasta)."),
            QStringLiteral("𝐓𝐚𝐦𝐚𝐧𝐡𝐨: Exibido em unidades legíveis (KB/MB/GB)."),
            QStringLiteral("𝐃𝐚𝐭𝐚: Data da última modificação ou evento registrado."),
            QStringLiteral("𝐄𝐯𝐞𝐧𝐭𝐨: Indica ações detectadas (criado, modificado, movido, excluído)."),
        },
            {}
        },
        {
            QStringLiteral("painel-detalhes"),
            QStringLiteral("Painel de Detalhes e Metadados"),
            {
            QStringLiteral("Quando um item é selecionado, o painel lateral mostra metadados relevantes e ações rápidas (abrir pasta, copiar caminho, atualizar metadados)."),
        },
            {
            QStringLiteral("𝐂𝐚𝐦𝐢𝐧𝐡𝐨 𝐜𝐨𝐦𝐩𝐥𝐞𝐭𝐨: Local do arquivo no disco."),
            QStringLiteral("𝐀𝐮𝐭𝐨𝐫 / 𝐂𝐫𝐢𝐚𝐝𝐨𝐫: Se presente nos metadados."),
            QStringLiteral("𝐃𝐢𝐦𝐞𝐧𝐬õ𝐞𝐬: Para imagens (largura × altura)."),
            QStringLiteral("𝐃𝐮𝐫𝐚çã𝐨: Para áudio/vídeo."),
            QStringLiteral("𝐓𝐚𝐱𝐚 𝐝𝐞 𝐛𝐢𝐭𝐬: Para mídias, quando disponível."),
            QStringLiteral("𝐏𝐞𝐫𝐦𝐢𝐬𝐬õ𝐞𝐬: Indica proteção contra alterações."),
        },
            {}
        },
        {
            QStringLiteral("acoes-sobre-arquivos"),
            QStringLiteral("Ações sobre Arquivos"),
            {},
            {
            QStringLiteral("𝐀𝐛𝐫𝐢𝐫: Abra com o aplicativo padrão (duplo clique ou botão)."),
            QStringLiteral("𝐂𝐨𝐩𝐢𝐚𝐫 / 𝐌𝐨𝐯𝐞𝐫: Use copiar/colar ou arrastar para mover arquivos entre pastas."),
            QStringLiteral("𝐑𝐞𝐧𝐨𝐦𝐞𝐚𝐫 / 𝐄𝐱𝐜𝐥𝐮𝐢𝐫: Acesse via menu de contexto; confirmações são solicitadas para ações permanentes."),
            QStringLiteral("𝐑𝐞𝐬𝐭𝐚𝐮𝐫𝐚𝐫: Quando disponível, recupere exclusões recentes pela função de restauração."),
            QStringLiteral("𝐏𝐫𝐨𝐩𝐫𝐢𝐞𝐝𝐚𝐝𝐞𝐬: Visualize metadados completos e histórico de eventos do arquivo."),
        },
            {}
        },
        {
            QStringLiteral("pesquisa-filtros"),
            QStringLiteral("Pesquisa e Filtros"),
            {},
            {
            QStringLiteral("𝐏𝐞𝐬𝐪𝐮𝐢𝐬𝐚: Use a caixa de busca para localizar itens por nome ou termo."),
            QStringLiteral("𝐅𝐢𝐥𝐭𝐫𝐨𝐬: Aplique filtros por tipo, tamanho, data ou evento para reduzir resultados."),
            QStringLiteral("𝐂𝐨𝐦𝐛𝐢𝐧𝐚𝐫 𝐟𝐢𝐥𝐭𝐫𝐨𝐬: Combine filtros para refinar a seleção."),
        },
            {}
        },
        {
            QStringLiteral("salvando-restaurando"),
            QStringLiteral("Salvar e Restaurar Visualização"),
            {},
            {
            QStringLiteral("𝐒𝐚𝐥𝐯𝐚𝐫 𝐞𝐬𝐭𝐚𝐝𝐨: Armazene a configuração atual de colunas e filtros como perfil."),
            QStringLiteral("𝐑𝐞𝐬𝐭𝐚𝐮𝐫𝐚𝐫: Carregue um perfil para recuperar layout e filtros salvos."),
        },
            {}
        },
        {
            QStringLiteral("processamento-lote"),
            QStringLiteral("Processamento em Lote"),
            {
            QStringLiteral("Selecione vários itens para extrair metadados ou executar ações em massa. O progresso é exibido e a operação pode ser pausada ou cancelada."),
        },
            {
            QStringLiteral("𝐈𝐧𝐢𝐜𝐢𝐚𝐫 𝐞𝐱𝐭𝐫𝐚çã𝐨: Selecione itens e escolha 'Extrair Metadados em Lote'."),
            QStringLiteral("𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐚𝐫 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐬𝐨: Acompanhe pela barra de progresso e pelo painel de mensagens."),
            QStringLiteral("𝐂𝐚𝐧𝐜𝐞𝐥𝐚𝐫/𝐏𝐚𝐮𝐬𝐚𝐫: Use os controles de processo para interromper ou pausar a operação."),
        },
            {}
        },
        {
            QStringLiteral("relatorios-estatisticas"),
            QStringLiteral("Relatórios e Estatísticas"),
            {
            QStringLiteral("A seção de estatísticas gera gráficos sobre tipos de arquivo, uso de disco e eventos. Exporte imagens ou CSV para análise externa."),
        },
            {
            QStringLiteral("𝐆𝐞𝐫𝐚𝐫 𝐠𝐫á𝐟𝐢𝐜𝐨𝐬: Selecione métricas para visualizar distribuições e tendências."),
            QStringLiteral("𝐄𝐱𝐩𝐨𝐫𝐭𝐚𝐫: Salve gráficos como imagens ou exporte dados em CSV."),
        },
            {}
        },
        {
            QStringLiteral("monitoramento-eventos"),
            QStringLiteral("Monitoramento de Eventos"),
            {},
            {
            QStringLiteral("𝐓𝐢𝐩𝐨𝐬 𝐝𝐞 𝐞𝐯𝐞𝐧𝐭𝐨: Criação, modificação, remoção e movimentação."),
            QStringLiteral("𝐕𝐢𝐬𝐮𝐚𝐥𝐢𝐳𝐚çã𝐨 𝐞𝐦 𝐭𝐞𝐦𝐩𝐨 𝐫𝐞𝐚𝐥: Eventos recentes são destacados para análise imediata."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐚çõ𝐞𝐬: Configure alertas visuais ou sonoros nas Preferências."),
        },
            {}
        },
        {
            QStringLiteral("preferencias-configuracoes"),
            QStringLiteral("Preferências"),
            {
            QStringLiteral("No painel de Preferências ajuste idioma, comportamento de scan, notificações e atualizações. Revise essas opções ao configurar o aplicativo."),
        },
            {
            QStringLiteral("𝐈𝐝𝐢𝐨𝐦𝐚: Escolha o idioma da interface."),
            QStringLiteral("𝐒𝐜𝐚𝐧: Configure periodicidade, profundidade e exclusões."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐚çõ𝐞𝐬: Ative ou desative alertas."),
            QStringLiteral("𝐀𝐭𝐮𝐚𝐥𝐢𝐳𝐚çõ𝐞𝐬: Defina verificação automática ou manual."),
        },
            {}
        },
        {
            QStringLiteral("problemas-comuns"),
            QStringLiteral("Problemas Comuns e Soluções"),
            {},
            {
            QStringLiteral("𝐒𝐜𝐚𝐧 𝐭𝐫𝐚𝐯𝐚𝐝𝐨 𝐨𝐮 𝐞𝐫𝐫𝐨: Aguarde; se persistir, reinicie o scan ou o aplicativo."),
            QStringLiteral("𝐈𝐭𝐞𝐧𝐬 𝐚𝐮𝐬𝐞𝐧𝐭𝐞𝐬: Verifique se a pasta foi adicionada corretamente e as permissões de leitura."),
            QStringLiteral("𝐅𝐚𝐥𝐡𝐚 𝐚𝐨 𝐚𝐛𝐫𝐢𝐫: Confirme que existe um aplicativo associado ao tipo de arquivo."),
            QStringLiteral("𝐌𝐞𝐭𝐚𝐝𝐚𝐝𝐨𝐬 𝐟𝐚𝐥𝐭𝐚𝐧𝐝𝐨: Nem todos os arquivos têm metadados; use 'Atualizar metadados' ou processamento em lote."),
        },
            {}
        },
        {
            QStringLiteral("atalhos-uteis"),
            QStringLiteral("Atalhos"),
            {},
            shortcut_lines(QStringLiteral("pt_BR")),
            {}
        },
        {
            QStringLiteral("boas-praticas"),
            QStringLiteral("Boas Práticas"),
            {},
            {
            QStringLiteral("Faça um scan inicial em uma pasta pequena antes de operações maiores."),
            QStringLiteral("Salve estados e configurações antes de processos em lote."),
            QStringLiteral("Use filtros para focar apenas nos itens relevantes."),
        },
            {}
        },
        {
            QStringLiteral("logs-e-diagnostico"),
            QStringLiteral("Logs e diagnóstico"),
            {},
            {
            QStringLiteral("Os logs de execução podem ajudar a diagnosticar problemas; verifique o arquivo de log gerado pela aplicação em: ") + dataDir,
        },
            {}
        },
        {
            QStringLiteral("faq"),
            QStringLiteral("Perguntas frequentes (FAQ)"),
            {},
            {},
            {
        {
            QStringLiteral("Onde meus dados são salvos?"),
            {
            QStringLiteral("As tarefas e arquivos de configuração são armazenados no diretório: ") + dataDir,
            QStringLiteral("Consulte esse diretório para localizar arquivos de persistência e logs."),
        },
            {}
        },
    }
        },
        {
            QStringLiteral("suporte"),
            QStringLiteral("Como obter ajuda e suporte"),
            {},
            {
            QStringLiteral("Consulte a seção 'Sobre' dentro do aplicativo para informações oficiais e notas de versão."),
            QStringLiteral("Para problemas mais complexos, gere logs e envie-os ao suporte. Os arquivos de log estão em: ") + dataDir,
            QStringLiteral("Envie os arquivos de log e uma descrição detalhada do problema para o email de suporte: linceu_lighthouse@outlook.com."),
        },
            {}
        },
    };
}

}  // namespace source::ui::manual
