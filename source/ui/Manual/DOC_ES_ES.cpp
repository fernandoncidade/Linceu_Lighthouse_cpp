#include "DOC_ES_ES.h"

namespace source::ui::manual {

std::vector<ManualSection> doc_es_es() {
    const QString dataDir = data_dir();
    return {
        {
            QStringLiteral("nuevos-recursos-funcionalidades"),
            QStringLiteral("Nuevos recursos y funcionalidades"),
            {
                QStringLiteral("La versión 2026.10.21.0 introduce la herramienta dedicada de consola (linceu-lighthouse-cli), nuevos parámetros de línea de comandos para la aplicación gráfica, optimización de recursos visuales para pantallas de alta definición y formalización de términos bajo Licencia MIT."),
                QStringLiteral("La versión 2026.9.28.0 introdujo mejoras estructurales de confiabilidade en el monitoreo en tiempo real, carga asíncrona no bloqueante para sesiones en tiempo real, sincronización diferencial de tablas sin parpadeo en cuarentena y restauración con verificación criptográfica SHA-256."),
                QStringLiteral("Las siguientes instrucciones detallan cómo utilizar estas capacidades de manera segura, predecible y eficiente en la operación diaria."),
            },
            {
                QStringLiteral("Herramienta CLI autónoma (linceu-lighthouse-cli.exe): ejecución por terminal para escaneo, monitoreo, metadatos PE, auditoría de línea de base y cuarentena."),
                QStringLiteral("Parámetros de inicio en la aplicación gráfica: soporte para apertura directa de carpetas, perfiles guardados, escaneo sin monitoreo y exportación automática."),
                QStringLiteral("Optimización de recursos visuales: inicio más veloz y menor uso de memoria al prescindir de iconos obsoletos en favor de gráficos nativos High-DPI."),
                QStringLiteral("Sesiones en tiempo real no bloqueantes: carga en segundo plano con indicadores de progreso y cancelación segura."),
                QStringLiteral("Cuarentena y restauración inteligente: actualización diferencial de tablas que preserva selección, orden y posición de desplazamiento."),
                QStringLiteral("Restauración segura con selector de carpeta de destino y validación criptográfica de integridad SHA-256."),
                QStringLiteral("Centro de alertas con clasificación por niveles objetivos de riesgo y evaluación determinista de reglas de seguridad."),
                QStringLiteral("Gestión multidirectorio mejorada con perfiles persistentes de monitoreo simultáneo."),
            },
            {
                {
                    QStringLiteral("Flujo recomendado para monitoreo continuo"),
                    {
                        QStringLiteral("1) Añada los directorios objetivo y ejecute un escaneo inicial completo."),
                        QStringLiteral("2) Confirme que la tabla esté poblada y que el estado de la interfaz sea correcto."),
                        QStringLiteral("3) Active el monitoreo continuo para capturar nuevos eventos en tiempo real."),
                        QStringLiteral("4) Use filtros para reducir ruido y acelerar auditorías."),
                    },
                    {}
                },
                {
                    QStringLiteral("Uso seguro de pausar, reanudar y detener"),
                    {
                        QStringLiteral("En operaciones largas, pause el monitoreo para una revisión intermedia."),
                        QStringLiteral("Reanude solo después de validar el conjunto de datos visible."),
                        QStringLiteral("Antes de cerrar, detenga monitoreo/escaneo explícitamente y espere la actualización final de estado."),
                    },
                    {}
                },
                {
                    QStringLiteral("Cómo utilizar las Sesiones en Tiempo Real Asíncronas"),
                    {
                        QStringLiteral("Abra Configuraciones > Seguridad > Sesiones en tiempo real."),
                        QStringLiteral("El historial se carga en segundo plano mediante un hilo de trabajo dedicado, manteniendo la interfaz ágil incluso con miles de eventos."),
                        QStringLiteral("Revise las columnas: Actualizado en, Etiqueta de sesión, Total de eventos, Directorios monitorizados e Identificador de evidencia."),
                        QStringLiteral("Pase el cursor sobre los directorios para ver la lista completa de rutas cuando existan múltiples carpetas monitorizadas."),
                        QStringLiteral("Haga clic en Actualizar en cualquier momento para consultar nuevos bloques de eventos sin cerrar el diálogo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Cómo utilizar la Cuarentena y Restauración Inteligente"),
                    {
                        QStringLiteral("Abra Configuraciones > Seguridad > Cuarentena y restauración."),
                        QStringLiteral("Navegue entre las pestañas Cuarentena (archivos sospechosos aislados) y Versiones monitorizadas (instantáneas de línea de base criptográfica)."),
                        QStringLiteral("La actualización de la tabla es diferencial: los nuevos registros no alteran su selección activa ni la posición de desplazamiento."),
                        QStringLiteral("Seleccione un archivo y use Abrir carpeta seleccionada para localizarlo en el Explorador de archivos de Windows."),
                        QStringLiteral("Pulse Restaurar copia seleccionada (o Restaurar versión monitorada) para elegir una carpeta segura de destino y recuperar el archivo verificado con integridad criptográfica."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("utilidad-linea-comandos-cli"),
            QStringLiteral("Utilidad de Línea de Comandos (CLI) e Automatización"),
            {
                QStringLiteral("El ejecutable 'linceu-lighthouse-cli.exe' es la herramienta de consola dedicada de la aplicación, diseñada para interactuar directamente en PowerShell, el Símbolo del sistema (CMD), el Programador de tareas de Windows (Task Scheduler) y flujos automatizados sin requerir interfaz visual."),
                QStringLiteral("Tanto la aplicación gráfica (Linceu_Lighthouse.exe) como la utilidad CLI funcionan 100% en local, compartiendo la misma infraestructura criptográfica, motor determinista de reglas de seguridad y base de datos SQLite en %LOCALAPPDATA%/LinceuLighthouse/monitoramento.db."),
            },
            {
                QStringLiteral("Sintaxis básica de la CLI: linceu-lighthouse-cli.exe <comando> [opciones]"),
                QStringLiteral("Comandos disponibles: scan, monitor, metadata, baseline, alerts, quarantine y sessions."),
                QStringLiteral("Opciones globales: --format <text|json|csv>, --depth <n>, --risks-only, --duration <segundos>, --no-banner, --help y --version."),
                QStringLiteral("Parámetros en la interfaz gráfica: Linceu_Lighthouse.exe [ruta], --dir <ruta>, --profile <nombre>, --scan-only y --export <formato> <archivo>."),
                QStringLiteral("Optimización gráfica: inicio ágil y menor consumo de memoria al retirar iconos heredados de baja resolución a favor de gráficos High-DPI."),
                QStringLiteral("Licencia MIT: libertad de uso e integración para el código fuente, preservando los derechos de distribución de binarios oficiales."),
            },
            {
                {
                    QStringLiteral("Cómo ejecutar escaneos por terminal (scan)"),
                    {
                        QStringLiteral("El comando 'scan' cataloga e inspecciona archivos en la ruta especificada, calculando hashes SHA-256 y nivel de riesgo."),
                        QStringLiteral("Escaneo básico: linceu-lighthouse-cli.exe scan \"C:\\MisDatos\""),
                        QStringLiteral("Escaneo con profundidad limitada y salida JSON: linceu-lighthouse-cli.exe scan \"C:\\MisDatos\" --depth 2 --format json"),
                        QStringLiteral("Escaneo filtrado solo para elementos con riesgo: linceu-lighthouse-cli.exe scan \"C:\\MisDatos\" --risks-only"),
                    },
                    {}
                },
                {
                    QStringLiteral("Cómo monitorizar eventos en tiempo real en consola (monitor)"),
                    {
                        QStringLiteral("El comando 'monitor' vigila cambios en el sistema de archivos de forma continua mediante APIs nativas de Windows."),
                        QStringLiteral("Monitoreo continuo: linceu-lighthouse-cli.exe monitor \"C:\\inetpub\\wwwroot\""),
                        QStringLiteral("Monitoreo programado por tiempo (ej: 1 hora): linceu-lighthouse-cli.exe monitor \"D:\\Datos\" --duration 3600"),
                        QStringLiteral("Presione Ctrl+C en cualquier momento para concluir la sesión de forma segura."),
                    },
                    {}
                },
                {
                    QStringLiteral("Cómo inspeccionar metadados y firmas PE (metadata)"),
                    {
                        QStringLiteral("El comando 'metadata' extrae propiedades técnicas profundas de ejecutables (arquitectura, secciones, sellos temporales y firmas Authenticode)."),
                        QStringLiteral("Ejemplo: linceu-lighthouse-cli.exe metadata \"C:\\Windows\\notepad.exe\""),
                    },
                    {}
                },
                {
                    QStringLiteral("Cómo auditar desviaciones de línea de base (baseline)"),
                    {
                        QStringLiteral("El comando 'baseline' detecta discrepancias comparando el estado actual de los archivos con el registro criptográfico SQLite."),
                        QStringLiteral("Auditar desviaciones: linceu-lighthouse-cli.exe baseline \"C:\\Configuraciones\""),
                        QStringLiteral("Registrar o actualizar línea de base: linceu-lighthouse-cli.exe baseline \"C:\\Configuraciones\" --update"),
                    },
                    {}
                },
                {
                    QStringLiteral("Cómo gestionar alertas y cuarentena por terminal (alerts y quarantine)"),
                    {
                        QStringLiteral("Consultar incidentes de seguridad: linceu-lighthouse-cli.exe alerts --format json"),
                        QStringLiteral("Listar archivos aislados en cuarentena: linceu-lighthouse-cli.exe quarantine list"),
                        QStringLiteral("Restaurar archivo con validación criptográfica: linceu-lighthouse-cli.exe quarantine restore <id> \"C:\\DestinoSeguro\""),
                    },
                    {}
                },
                {
                    QStringLiteral("Parámetros útiles en la aplicación de escritorio (Linceu_Lighthouse.exe)"),
                    {
                        QStringLiteral("Apertura directa de un directorio: Linceu_Lighthouse.exe \"D:\\Proyectos\""),
                        QStringLiteral("Carga automática de perfil: Linceu_Lighthouse.exe --profile \"AuditoriaServidores\""),
                        QStringLiteral("Escaneo silencioso y exportación inmediata: Linceu_Lighthouse.exe --dir \"C:\\Datos\" --scan-only --export xlsx informe.xlsx"),
                        QStringLiteral("Selección del controlador de gráficos con la variable LINCEU_OPENGL_MODE (auto, desktop, angle, software)."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("abrir-archivos-mime-fechas-carpetas"),
            QStringLiteral("Apertura de archivos, MIME y fechas de carpetas"),
            {
                QStringLiteral("La tabla ahora ayuda a abrir los elementos reportados aunque no sean archivos de texto. Al usar Abrir o doble clic en una fila de archivo, la aplicación intenta usar la aplicación predeterminada de Windows y puede ofrecer otras opciones compatibles."),
                QStringLiteral("La columna MIME muestra el tipo real de contenido en lenguaje legible según el idioma actual, manteniendo el identificador original entre paréntesis para auditoría. Las columnas Fecha de creación, Fecha de modificación y Fecha de acceso también se rellenan para carpetas, subcarpetas y directorios hijos durante el escaneo."),
            },
            {
                QStringLiteral("Use Abrir o doble clic para abrir documentos, imágenes, vídeos, hojas de cálculo, ejecutables, archivos comprimidos y otros tipos reconocidos por el sistema."),
                QStringLiteral("Cuando Windows informe más de una opción, elija la aplicación deseada en la ventana de selección."),
                QStringLiteral("Active la columna MIME para revisar la familia y el subtipo de contenido detectado."),
                QStringLiteral("Active las tres columnas de fecha para auditar cuándo se crearon, modificaron o accedieron carpetas y archivos."),
            },
            {
                {
                    QStringLiteral("Cómo abrir cualquier tipo de archivo"),
                    {
                        QStringLiteral("1) Seleccione una fila que represente un archivo existente."),
                        QStringLiteral("2) Haga doble clic o use la acción Abrir."),
                        QStringLiteral("3) Si aparece una ventana de elección, seleccione la aplicación deseada y confirme."),
                        QStringLiteral("4) Si el elemento es una carpeta, use la acción de abrir carpeta para navegar hasta ella en el Explorador de archivos."),
                    },
                    {}
                },
                {
                    QStringLiteral("Cómo interpretar la columna MIME"),
                    {
                        QStringLiteral("La primera parte muestra la familia del contenido, como Texto, Imagen, Vídeo, Audio o Aplicación."),
                        QStringLiteral("La segunda parte muestra el subtipo en lenguaje simple, como PDF, PNG, ZIP o Texto sin formato."),
                        QStringLiteral("El texto entre paréntesis conserva el identificador original, por ejemplo text/plain o application/pdf."),
                    },
                    {}
                },
                {
                    QStringLiteral("Cómo revisar fechas en carpetas y subcarpetas"),
                    {
                        QStringLiteral("1) Abra el menú de columnas y active Fecha de creación, Fecha de modificación y Fecha de acceso."),
                        QStringLiteral("2) Ejecute un nuevo escaneo en el directorio elegido."),
                        QStringLiteral("3) Ordene o filtre la tabla para revisar carpetas hijas y directorios más profundos."),
                        QStringLiteral("4) Use estas fechas para comparar cambios recientes con eventos registrados."),
                    },
                    {}
                },
                {
                    QStringLiteral("Si un archivo no se abre"),
                    {
                        QStringLiteral("Confirme que la ruta todavía exista y que tenga permiso de lectura."),
                        QStringLiteral("Verifique si Windows tiene una aplicación asociada a ese tipo de archivo."),
                        QStringLiteral("Cuando sea necesario, elija manualmente otra aplicación compatible en la ventana de selección."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("operacion-alto-volumen"),
            QStringLiteral("Operación con alto volumen"),
            {
                QStringLiteral("Cuando muchos archivos cambian en secuencia, trabaje en ciclos cortos: capturar, revisar, filtrar y exportar."),
            },
            {
                QStringLiteral("Ordene por fecha/evento para priorizar cambios recientes."),
                QStringLiteral("Aplique filtros por directorio/tipo antes de exportar datos."),
                QStringLiteral("Use pausas estratégicas para crear puntos claros de revisión."),
            },
            {
                {
                    QStringLiteral("Cambio de idioma durante alta actividad"),
                    {
                        QStringLiteral("Es posible cambiar idioma con monitoreo activo, pero se recomienda pausar durante validaciones visuales críticas."),
                        QStringLiteral("Después del cambio, valide textos de menús, encabezados de tabla, ayuda y estadísticas."),
                    },
                    {}
                },
                {
                    QStringLiteral("Validación rápida de estabilidad"),
                    {
                        QStringLiteral("Verifique que sigan apareciendo eventos nuevos después de reanudar el monitoreo."),
                        QStringLiteral("Confirme que etiquetas de estado y progreso cambien correctamente entre estados."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("recursos-complementarios-uso"),
            QStringLiteral("Recursos complementarios de uso"),
            {
                QStringLiteral("Esta versión también incorpora recursos prácticos para el uso diario: configuración avanzada del popup de seguridad, clasificación refinada de eventos y apertura de estadísticas con verificación de consistencia."),
            },
            {
                QStringLiteral("Controles del popup de seguridad: umbral mínimo, máximo de líneas e intervalo entre alertas."),
                QStringLiteral("Separación más clara entre eventos Renombrado y Movido para auditoría."),
                QStringLiteral("Flujo de estadísticas con mayor consistencia cuando aún hay escrituras pendientes."),
            },
            {
                {
                    QStringLiteral("Configurar el popup de seguridad"),
                    {
                        QStringLiteral("Abra el menú Seguridad y entre en la configuración de popup/notificaciones."),
                        QStringLiteral("Defina el umbral mínimo para disparar alertas."),
                        QStringLiteral("Ajuste el máximo de líneas mostradas por popup."),
                        QStringLiteral("Configure el intervalo entre alertas y guarde las preferencias."),
                    },
                    {}
                },
                {
                    QStringLiteral("Interpretar Renombrado vs Movido"),
                    {
                        QStringLiteral("Renombrado: mismo directorio, cambia solo el nombre del elemento."),
                        QStringLiteral("Movido: cambia de directorio, con o sin cambio de nombre."),
                        QStringLiteral("Para auditorías, filtre por Evento y revise rutas de origen/destino en el panel de detalles."),
                    },
                    {}
                },
                {
                    QStringLiteral("Abrir estadísticas con datos estables"),
                    {
                        QStringLiteral("Finalice operaciones pesadas o pause el monitoreo antes de abrir estadísticas."),
                        QStringLiteral("Después de abrir, valide que no existan estados pendientes de procesamiento."),
                        QStringLiteral("Si detecta inconsistencias, cierre y vuelva a abrir tras estabilizar el procesamiento."),
                    },
                    {}
                },
                {
                    QStringLiteral("Modo gráfico avanzado (opcional)"),
                    {
                        QStringLiteral("Si hay problemas de compatibilidad gráfica, configure LINCEU_OPENGL_MODE antes de iniciar la aplicación."),
                        QStringLiteral("Valores permitidos: auto, desktop, angle y software."),
                        QStringLiteral("Use software solo para diagnóstico, ya que puede reducir el rendimiento."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("flujo-operativo-menus-actualizados"),
            QStringLiteral("Flujo operativo de los menús actualizados"),
            {
                QStringLiteral("Los menús Archivo, Configuraciones y Opciones ahora cubren el ciclo completo: seleccionar directorios, iniciar o pausar análisis, preparar la tabla, configurar estadísticas, exportar datos y revisar recursos de seguridad."),
            },
            {
                QStringLiteral("Use Archivo > Seleccionar directorio o Ctrl+Shift+O antes del escaneo inicial."),
                QStringLiteral("Use Archivo > Iniciar/Detener o Ctrl+R para controlar el análisis, y Archivo > Pausar o Ctrl+P para crear un punto de revisión sin limpiar el estado actual."),
                QStringLiteral("Use Configuraciones > Filtros, Columnas y Columnas coloreadas antes de exportar o abrir estadísticas, para que la tabla destaque solo datos relevantes."),
                QStringLiteral("Use Opciones > Manual cuando necesite volver a abrir estas instrucciones sin reiniciar la aplicación."),
            },
            {
                {
                    QStringLiteral("Secuencia recomendada"),
                    {
                        QStringLiteral("1. Seleccione un directorio y ejecute el escaneo inicial."),
                        QStringLiteral("2. Ajuste filtros, columnas y colores para dejar visibles solo los datos útiles."),
                        QStringLiteral("3. Abra Configuraciones > Estadísticas > Trazar gráficos y elija los gráficos necesarios para la auditoría."),
                        QStringLiteral("4. Abra Archivo > Estadísticas o pulse Ctrl+G para generar el análisis visual."),
                        QStringLiteral("5. Exporte resultados solo después de confirmar que tabla y gráficos muestran el recorte esperado."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-menu-estadisticas-graficos"),
            QStringLiteral("Cómo usar Configuraciones > Estadísticas > Trazar gráficos"),
            {
                QStringLiteral("Este submenú controla qué gráficos se generan en la ventana de estadísticas. Los elementos marcados se guardan en statistics_charts_config.json dentro de la carpeta persistente de la aplicación y se reutilizan al abrir estadísticas."),
            },
            {
                QStringLiteral("Marque solo los gráficos que desea generar para reducir tiempo de procesamiento y mantener la ventana enfocada."),
                QStringLiteral("Desmarque gráficos pesados o innecesarios al analizar muchos eventos."),
                QStringLiteral("Después de cambiar la selección, abra Archivo > Estadísticas o pulse Ctrl+G para generar la ventana con el conjunto elegido."),
            },
            {
                {
                    QStringLiteral("Elegir grupos de gráficos"),
                    {
                        QStringLiteral("Use Pastel, Barras, Pareto y Distribuciones para comparar volúmenes por tipo, tamaño u operación."),
                        QStringLiteral("Use Línea temporal, Operaciones por día, Semana/Hora y Heatmap para investigar concentración temporal."),
                        QStringLiteral("Use Sankey, Evento por directorio y Tipo por directorio para descubrir flujos entre operación, tipo de archivo y carpeta monitorizada."),
                        QStringLiteral("Use Árbol de directorios para ubicar qué niveles de carpeta concentran más actividad."),
                        QStringLiteral("Use Scatter, Radar y Dotplot para comparar tamaños, frecuencia y distribución de operaciones."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-ventana-estadisticas"),
            QStringLiteral("Cómo operar la ventana de Estadísticas"),
            {
                QStringLiteral("La ventana de estadísticas lee la base de monitoreo y muestra los gráficos seleccionados. Ábrala solo cuando la tabla tenga datos; durante escaneos muy pesados, espere a que el estado se estabilice antes de actualizar."),
            },
            {
                QStringLiteral("Use el panel Seleccionar gráficos para mostrar u ocultar gráficos durante la sesión actual."),
                QStringLiteral("Use Seleccionar todos para activar todos los gráficos disponibles en la ventana."),
                QStringLiteral("Use Guardar seleccionados para exportar solo los gráficos marcados como PNG."),
                QStringLiteral("Use Guardar todos para exportar todos los gráficos generados a una carpeta con fecha y hora."),
                QStringLiteral("Use Actualizar después de nuevos eventos, cambios relevantes de filtros o cambios en el menú de selección de gráficos."),
                QStringLiteral("Oculte el panel de selección cuando necesite más espacio para comparar pestañas y detalles visuales."),
            },
            {}
        },
        {
            QStringLiteral("uso-menu-seguridad"),
            QStringLiteral("Cómo usar Configuraciones > Seguridad"),
            {
                QStringLiteral("El submenú Seguridad reúne recursos de auditoría, seguimiento de sesiones en tiempo real, cuarentena, restauración, líneas de base criptográficas y paquetes de evidencia. Úselo después de configurar directorios y preferiblemente tras un escaneo inicial."),
            },
            {},
            {
                {
                    QStringLiteral("Perfiles y multidirectorio"),
                    {
                        QStringLiteral("Abra Configuraciones > Seguridad > Perfiles y multidirectorio para gestionar el perfil activo."),
                        QStringLiteral("Use Añadir directorio para incluir otra carpeta en el perfil y Remover seleccionado para quitar la carpeta destacada."),
                        QStringLiteral("Edite Nombre del perfil y pulse Guardar perfil para identificar el conjunto monitorizado."),
                        QStringLiteral("Pulse Actualizar para recargar directorio principal, total de directorios y perfil activo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Centro de alertas en tiempo real"),
                    {
                        QStringLiteral("Abra Configuraciones > Seguridad > Centro de alertas para revisar incidencias clasificadas por niveles de riesgo: Bajo, Medio, Alto y Crítico."),
                        QStringLiteral("Consulte el resumen de la alerta, reglas activadas, ruta, nombre de archivo, sesión asociada e identificador de evidencia."),
                        QStringLiteral("Priorice la investigación de alertas Críticas o Altas antes de revisar eventos comunes en la tabla principal."),
                        QStringLiteral("Pulse Actualizar para consultar nuevos eventos detectados sin interrumpir la monitorización activa."),
                    },
                    {}
                },
                {
                    QStringLiteral("Sesiones en tiempo real asíncronas"),
                    {
                        QStringLiteral("Abra Configuraciones > Seguridad > Sesiones en tiempo real para supervisar bloques de actividad del sistema de archivos."),
                        QStringLiteral("La carga se realiza en segundo plano mediante un hilo dedicado, evitando bloqueos o demoras visuales incluso con grandes volúmenes de historial."),
                        QStringLiteral("Compruebe la marca temporal, la etiqueta legible de sesión, el total de eventos registrados, el resumen de directorios y el identificador de sesión."),
                        QStringLiteral("Pase el cursor sobre los directorios para ver la lista completa de rutas cuando la sesión abarque múltiples carpetas."),
                        QStringLiteral("Use Actualizar para cargar nuevas sesiones incrementales mientras el monitoreo permanece activo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Cuarentena y restauración inteligente"),
                    {
                        QStringLiteral("Abra Configuraciones > Seguridad > Cuarentena y restauración para acceder a las pestañas Cuarentena y Versiones monitorizadas."),
                        QStringLiteral("Las tablas utilizan sincronización diferencial sin parpadeos: su selección activa, ordenación y posición de desplazamiento permanecen intactas."),
                        QStringLiteral("En la pestaña Cuarentena, seleccione un archivo aislado y use Abrir carpeta seleccionada para examinar su ubicación en el Explorador de Windows."),
                        QStringLiteral("Pulse Restaurar copia seleccionada para abrir el cuadro de diálogo de destino: restaure en la ruta original o elija una carpeta segura para análisis."),
                        QStringLiteral("En la pestaña Versiones monitorizadas, seleccione una instantánea de la línea de base criptográfica y use Restaurar versión monitorizada para recuperarla."),
                        QStringLiteral("El sistema valida la integridad criptográfica SHA-256 y muestra una confirmación inmediata de éxito tras el restablecimiento."),
                    },
                    {}
                },
                {
                    QStringLiteral("Paquete de evidencia y auditoría"),
                    {
                        QStringLiteral("Abra Configuraciones > Seguridad > Paquete de evidencia para generar un conjunto de auditoría estructurado."),
                        QStringLiteral("Elija la carpeta de destino, pulse Exportar ahora y examine el paquete generado con líneas de base criptográficas y registros de eventos."),
                        QStringLiteral("Use Limpiar base de seguridad únicamente tras haber exportado evidencias y confirmar que el historial anterior ya no es necesario."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-notificaciones-exportacion"),
            QStringLiteral("Cómo usar notificaciones y opciones de exportación"),
            {
                QStringLiteral("Las notificaciones de seguridad y las opciones de exportación ayudan a controlar volumen de información, frecuencia de avisos y formato de los datos guardados."),
            },
            {
                QStringLiteral("Abra Configuraciones > Seguridad > Notificaciones para activar resúmenes en pie/estado o configurar tipos de pop-up."),
                QStringLiteral("En pop-ups de alertas, sesiones o cuarentena, use Activar este pop-up, Disparar al alcanzar, Máximo de líneas en el resumen y Cooldown entre pop-ups para ajustar la frecuencia."),
                QStringLiteral("Abra Configuraciones > Opciones de exportación para decidir si el archivo respeta columnas activas, filtros activos o solo filas seleccionadas."),
                QStringLiteral("Use Redefinir opciones de exportación para volver al comportamiento estándar antes de generar CSV, TXT, JSON, XML, DB, XLSX o paquetes de evidencia."),
            },
            {}
        },
        {
            QStringLiteral("vision-general"),
            QStringLiteral("Visión General"),
            {
            QStringLiteral("Linceu Lighthouse le ayuda a inspeccionar, monitorizar y analizar archivos y carpetas en varias ubicaciones de su equipo. Centraliza la información de los elementos y ofrece herramientas para filtrar, generar estadísticas y extraer metadatos."),
        },
            {},
            {}
        },
        {
            QStringLiteral("pantalla-principal"),
            QStringLiteral("Pantalla Principal — Elementos"),
            {},
            {
            QStringLiteral("𝐁𝐚𝐫𝐫𝐚 𝐝𝐞 𝐡𝐞𝐫𝐫𝐚𝐦𝐢𝐞𝐧𝐭𝐚𝐬: Acceso rápido a: Añadir Directorio, Iniciar Escaneo, Detener, Actualizar y Preferencias."),
            QStringLiteral("𝐒𝐞𝐥𝐞𝐜𝐭𝐨𝐫 𝐝𝐞 𝐝𝐢𝐫𝐞𝐜𝐭𝐨𝐫𝐢𝐨𝐬: Área para añadir, eliminar o cambiar carpetas monitorizadas."),
            QStringLiteral("𝐓𝐚𝐛𝐥𝐚 𝐝𝐞 𝐞𝐥𝐞𝐦𝐞𝐧𝐭𝐨𝐬: Lista central con columnas (Nombre, Tipo, Tamaño, Fecha, Evento) con ordenación/filtrado."),
            QStringLiteral("𝐏𝐚𝐧𝐞𝐥 𝐝𝐞 𝐝𝐞𝐭𝐚𝐥𝐥𝐞𝐬: Muestra metadatos y acciones rápidas para el elemento seleccionado."),
            QStringLiteral("𝐌𝐞𝐧𝐬𝐚𝐣𝐞𝐬 𝐲 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐨: Muestra notificaciones, registros de operación y barras de progreso."),
            QStringLiteral("𝐄𝐬𝐭𝐚𝐝í𝐬𝐭𝐢𝐜𝐚𝐬: Área para gráficos e informes generados a partir de los datos recopilados."),
        },
            {}
        },
        {
            QStringLiteral("comenzando"),
            QStringLiteral("Cómo empezar"),
            {},
            {
            QStringLiteral("Abra la aplicación desde el acceso directo o ejecutable."),
            QStringLiteral("Haga clic en 𝐀ñ𝐚𝐝𝐢𝐫 𝐃𝐢𝐫𝐞𝐜𝐭𝐨𝐫𝐢𝐨 y elija la carpeta a monitorizar."),
            QStringLiteral("Ejecute un escaneo inicial con 𝐈𝐧𝐢𝐜𝐢𝐚𝐫/𝐄𝐬𝐜𝐚𝐧𝐞𝐨."),
            QStringLiteral("Espere a que finalice; los elementos aparecerán con los metadatos disponibles."),
            QStringLiteral("Active 𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐢𝐳𝐚𝐜𝐢ó𝐧 𝐜𝐨𝐧𝐭𝐢𝐧𝐮𝐚 para detectar cambios automáticamente."),
        },
            {}
        },
        {
            QStringLiteral("modos-operacion"),
            QStringLiteral("Modos de operación"),
            {},
            {
            QStringLiteral("𝐄𝐬𝐜𝐚𝐧𝐞𝐨 ú𝐧𝐢𝐜𝐨: Verificación inmediata de la(s) carpeta(s) seleccionada(s)."),
            QStringLiteral("𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐢𝐳𝐚𝐜𝐢ó𝐧 𝐜𝐨𝐧𝐭𝐢𝐧𝐮𝐚: Vigilar cambios en tiempo real y actualizar la tabla automáticamente."),
            QStringLiteral("𝐏𝐫𝐨𝐜𝐞𝐬𝐚𝐦𝐢𝐞𝐧𝐭𝐨 𝐩𝐨𝐫 𝐥𝐨𝐭𝐞𝐬: Extraer metadatos o ejecutar acciones en varios archivos a la vez."),
        },
            {}
        },
        {
            QStringLiteral("tabla-elementos"),
            QStringLiteral("Entendiendo la tabla de elementos"),
            {
            QStringLiteral("Cada columna muestra una propiedad del elemento. Use ordenación, filtros y selección para localizar e inspeccionar elementos eficazmente."),
        },
            {
            QStringLiteral("𝐍𝐨𝐦𝐛𝐫𝐞: Nombre de archivo o carpeta; doble clic para abrir."),
            QStringLiteral("𝐓𝐢𝐩𝐨: Tipo detectado (imagen, vídeo, documento, carpeta)."),
            QStringLiteral("𝐓𝐚𝐦𝐚ñ𝐨: Mostrado en unidades legibles (KB/MB/GB)."),
            QStringLiteral("𝐅𝐞𝐜𝐡𝐚: Fecha de la última modificación o evento registrado."),
            QStringLiteral("𝐄𝐯𝐞𝐧𝐭𝐨: Acción detectada (creado, modificado, movido, eliminado)."),
        },
            {}
        },
        {
            QStringLiteral("panel-detalles"),
            QStringLiteral("Panel de detalles y metadatos"),
            {
            QStringLiteral("Al seleccionar un elemento, el panel lateral muestra metadatos relevantes y acciones rápidas (abrir carpeta, copiar ruta, actualizar metadatos)."),
        },
            {
            QStringLiteral("𝐑𝐮𝐭𝐚 𝐜𝐨𝐦𝐩𝐥𝐞𝐭𝐚: Ubicación del archivo en disco."),
            QStringLiteral("𝐀𝐮𝐭𝐨𝐫 / 𝐂𝐫𝐞𝐚𝐝𝐨𝐫: Cuando esté presente en los metadatos."),
            QStringLiteral("𝐃𝐢𝐦𝐞𝐧𝐬𝐢𝐨𝐧𝐞𝐬: Para imágenes (ancho × alto)."),
            QStringLiteral("𝐃𝐮𝐫𝐚𝐜𝐢ó𝐧: Para audio/video."),
            QStringLiteral("𝐓𝐚𝐬𝐚 𝐝𝐞 𝐛𝐢𝐭𝐬: Para medios, cuando esté disponible."),
            QStringLiteral("𝐏𝐞𝐫𝐦𝐢𝐬𝐨𝐬: Indica protección o restricciones de escritura."),
        },
            {}
        },
        {
            QStringLiteral("acciones-archivo"),
            QStringLiteral("Acciones sobre archivos"),
            {},
            {
            QStringLiteral("𝐀𝐛𝐫𝐢𝐫: Abrir con la aplicación predeterminada del sistema (doble clic o botón Abrir)."),
            QStringLiteral("𝐂𝐨𝐩𝐢𝐚𝐫 / 𝐌𝐨𝐯𝐞𝐫: Use copiar/pegar o arrastrar y soltar para mover archivos entre carpetas."),
            QStringLiteral("𝐑𝐞𝐧𝐨𝐦𝐛𝐫𝐚𝐫 / 𝐄𝐥𝐢𝐦𝐢𝐧𝐚𝐫: Use el menú contextual; las acciones permanentes requieren confirmación."),
            QStringLiteral("𝐑𝐞𝐬𝐭𝐚𝐮𝐫𝐚𝐫: Cuando esté disponible, recupere eliminaciones recientes mediante la función de restauración."),
            QStringLiteral("𝐏𝐫𝐨𝐩𝐢𝐞𝐝𝐚𝐝𝐞𝐬: Ver metadatos completos e historial de eventos del archivo."),
        },
            {}
        },
        {
            QStringLiteral("busqueda-filtros"),
            QStringLiteral("Búsqueda y filtros"),
            {},
            {
            QStringLiteral("𝐁𝐮𝐬𝐜𝐚𝐫: Use la caja de búsqueda para localizar elementos por nombre o término."),
            QStringLiteral("𝐅𝐢𝐥𝐭𝐫𝐨𝐬: Aplique filtros por tipo, tamaño, fecha o evento para reducir resultados."),
            QStringLiteral("𝐂𝐨𝐦𝐛𝐢𝐧𝐚𝐫 𝐟𝐢𝐥𝐭𝐫𝐨𝐬: Combine filtros para refinar la selección."),
        },
            {}
        },
        {
            QStringLiteral("guardar-restaurar"),
            QStringLiteral("Guardar y restaurar vista"),
            {},
            {
            QStringLiteral("𝐆𝐮𝐚𝐫𝐝𝐚𝐫 𝐞𝐬𝐭𝐚𝐝𝐨: Almacene la configuración actual de columnas y filtros como perfil."),
            QStringLiteral("𝐑𝐞𝐬𝐭𝐚𝐮𝐫𝐚𝐫 𝐞𝐬𝐭𝐚𝐝𝐨: Cargue un perfil guardado para recuperar el diseño y filtros."),
        },
            {}
        },
        {
            QStringLiteral("procesamiento-lote"),
            QStringLiteral("Procesamiento por lotes"),
            {
            QStringLiteral("Seleccione varios elementos para extraer metadatos o ejecutar acciones en lote. El progreso y los controles permiten pausar o cancelar."),
        },
            {
            QStringLiteral("𝐈𝐧𝐢𝐜𝐢𝐚𝐫 𝐞𝐱𝐭𝐫𝐚𝐜𝐜𝐢ó𝐧: Elija elementos y ejecute la extracción de metadatos por lote."),
            QStringLiteral("𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐞𝐚𝐫 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐨: Supervise mediante la barra de progreso y el panel de mensajes."),
            QStringLiteral("𝐏𝐚𝐮𝐬𝐚𝐫/𝐂𝐚𝐧𝐜𝐞𝐥𝐚𝐫: Use los controles para interrumpir el procesamiento."),
        },
            {}
        },
        {
            QStringLiteral("informes-estadisticas"),
            QStringLiteral("Informes y estadísticas"),
            {
            QStringLiteral("La sección de estadísticas produce gráficos sobre tipos de archivos, uso de disco y tendencias de eventos. Exporte imágenes o CSV para uso externo."),
        },
            {
            QStringLiteral("𝐆𝐞𝐧𝐞𝐫𝐚𝐫 𝐠𝐫á𝐟𝐢𝐜𝐨𝐬: Seleccione métricas para visualizar distribuciones y tendencias."),
            QStringLiteral("𝐄𝐱𝐩𝐨𝐫𝐭𝐚𝐫: Guarde gráficos como imágenes o exporte datos en CSV."),
        },
            {}
        },
        {
            QStringLiteral("monitorizacion-eventos"),
            QStringLiteral("Monitorización de eventos"),
            {},
            {
            QStringLiteral("𝐓𝐢𝐩𝐨𝐬 𝐝𝐞 𝐞𝐯𝐞𝐧𝐭𝐨: Creación, modificación, eliminación y movimiento."),
            QStringLiteral("𝐕𝐢𝐬𝐭𝐚 𝐞𝐧 𝐭𝐢𝐞𝐦𝐩𝐨 𝐫𝐞𝐚𝐥: Los eventos recientes se resaltan para inspección rápida."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐚𝐜𝐢𝐨𝐧𝐞𝐬: Configure alertas visuales o sonoras en Preferencias."),
        },
            {}
        },
        {
            QStringLiteral("preferencias"),
            QStringLiteral("Preferencias"),
            {
            QStringLiteral("Ajuste idioma, comportamiento de escaneo, notificaciones y actualizaciones. Revise estas opciones después de la instalación."),
        },
            {
            QStringLiteral("𝐈𝐝𝐢𝐨𝐦𝐚: Elija el idioma de la interfaz."),
            QStringLiteral("𝐄𝐬𝐜𝐚𝐧𝐞𝐨: Configure periodicidad, profundidad y exclusiones."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐚𝐜𝐢𝐨𝐧𝐞𝐬: Active o desactive alertas."),
            QStringLiteral("𝐀𝐜𝐭𝐮𝐚𝐥𝐢𝐳𝐚𝐜𝐢𝐨𝐧𝐞𝐬: Elija comprobaciones automáticas o manuales."),
        },
            {}
        },
        {
            QStringLiteral("problemas-comunes"),
            QStringLiteral("Problemas comunes y soluciones"),
            {},
            {
            QStringLiteral("𝐄𝐬𝐜𝐚𝐧𝐞𝐨 𝐛𝐥𝐨𝐪𝐮𝐞𝐚𝐝𝐨 𝐨 𝐞𝐫𝐫𝐨𝐫: Espere brevemente; si persiste, reinicie el escaneo o la aplicación."),
            QStringLiteral("𝐄𝐥𝐞𝐦𝐞𝐧𝐭𝐨𝐬 𝐟𝐚𝐥𝐭𝐚𝐧𝐭𝐞𝐬: Verifique que la carpeta esté añadida y que la aplicación tenga permisos de lectura."),
            QStringLiteral("𝐍𝐨 𝐬𝐞 𝐩𝐮𝐞𝐝𝐞 𝐚𝐛𝐫𝐢𝐫: Asegúrese de que exista una aplicación asociada al tipo de archivo."),
            QStringLiteral("𝐌𝐞𝐭𝐚𝐝𝐚𝐭𝐨𝐬 𝐟𝐚𝐥𝐭𝐚𝐧𝐭𝐞𝐬: No todos los archivos incluyen metadatos; intente actualizar o procesar por lotes."),
        },
            {}
        },
        {
            QStringLiteral("atajos"),
            QStringLiteral("Atajos"),
            {},
            shortcut_lines(QStringLiteral("es_ES")),
            {}
        },
        {
            QStringLiteral("buenas-practicas"),
            QStringLiteral("Buenas prácticas"),
            {},
            {
            QStringLiteral("Ejecute un escaneo inicial en una carpeta pequeña antes de operaciones grandes."),
            QStringLiteral("Guarde estados y configuraciones antes de tareas por lotes."),
            QStringLiteral("Use filtros para centrarse en elementos relevantes."),
        },
            {}
        },
        {
            QStringLiteral("logs-diagnostico"),
            QStringLiteral("Registros y diagnóstico"),
            {},
            {
            QStringLiteral("Los registros de la aplicación pueden ayudar a diagnosticar problemas; consulte el archivo de registro generado por la aplicación en: ") + dataDir,
        },
            {}
        },
        {
            QStringLiteral("faq-es"),
            QStringLiteral("Preguntas frecuentes (FAQ)"),
            {},
            {},
            {
        {
            QStringLiteral("¿Dónde se guardan mis datos?"),
            {
            QStringLiteral("Las tareas y archivos de configuración se almacenan en: ") + dataDir,
            QStringLiteral("Consulte ese directorio para localizar archivos de persistencia y registros."),
        },
            {}
        },
    }
        },
        {
            QStringLiteral("soporte"),
            QStringLiteral("Cómo obtener ayuda y soporte"),
            {},
            {
            QStringLiteral("Consulte la sección Acerca de dentro de la aplicación para información oficial y notas de la versión."),
            QStringLiteral("Para problemas complejos, genere registros y envíelos al soporte. Los archivos de registro están en: ") + dataDir,
            QStringLiteral("Envíe los registros y una descripción detallada del problema al correo de soporte: linceu_lighthouse@outlook.com."),
        },
            {}
        },
    };
}

}  // namespace source::ui::manual
