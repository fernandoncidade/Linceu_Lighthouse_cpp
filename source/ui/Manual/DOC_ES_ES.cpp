#include "DOC_ES_ES.h"

namespace source::ui::manual {

std::vector<ManualSection> doc_es_es() {
    const QString dataDir = data_dir();
    return {
        {
            QStringLiteral("nuevos-recursos-funcionalidades"),
            QStringLiteral("Nuevos recursos y funcionalidades"),
            {
                QStringLiteral("Se implementaron mejoras en la confiabilidad del monitoreo en tiempo real, el control de pausar/reanudar/detener y la actualización de la tabla en escenarios con alto volumen de eventos."),
                QStringLiteral("Las instrucciones siguientes explican cómo usar estos cambios de forma segura en la operación diaria."),
            },
            {
                QStringLiteral("Detección más precisa para operaciones de agregar, modificar, eliminar, renombrar y mover archivos."),
                QStringLiteral("Actualización visual más estable en listas grandes, con menor riesgo de inconsistencias temporales durante picos."),
                QStringLiteral("Cambio de idioma más consistente en menús, tabla, manual, ayuda y estadísticas sin reiniciar la aplicación."),
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
                QStringLiteral("El submenú Seguridad agrupa auditoría, sesiones en tiempo real, cuarentena, restauración y paquetes de evidencia. Úselo después de configurar directorios y preferiblemente tras un escaneo inicial."),
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
                    QStringLiteral("Centro de alertas y sesiones en tiempo real"),
                    {
                        QStringLiteral("Abra Centro de alertas para revisar hora, nivel de riesgo, resumen, reglas activadas, ruta, nombre, sesión y evidencia."),
                        QStringLiteral("Priorice alertas críticas o altas antes de revisar eventos comunes en la tabla principal."),
                        QStringLiteral("Abra Sesiones en tiempo real para inspeccionar sesiones recientes, total de eventos y directorios involucrados."),
                        QStringLiteral("Use Actualizar en cada ventana mientras el monitoreo esté activo para cargar nuevos registros sin cerrar el diálogo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Cuarentena, restauración y evidencia"),
                    {
                        QStringLiteral("Abra Cuarentena y restauración para revisar las pestañas Cuarentena y Versiones monitorizadas."),
                        QStringLiteral("En Cuarentena, seleccione una fila y use Abrir carpeta seleccionada para ubicar la copia aislada; restaure solo después de verificar el destino."),
                        QStringLiteral("En Versiones monitorizadas, seleccione una fila y use Restaurar versión monitorizada para recuperar una versión registrada."),
                        QStringLiteral("Abra Paquete de evidencia, elija destino, pulse Exportar ahora e inspeccione la carpeta generada."),
                        QStringLiteral("Use Limpiar base de seguridad solo después de exportar evidencia o confirmar que el historial antiguo ya no es necesario."),
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
