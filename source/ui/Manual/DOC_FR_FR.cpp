#include "DOC_FR_FR.h"

namespace source::ui::manual {

std::vector<ManualSection> doc_fr_fr() {
    const QString dataDir = data_dir();
    return {
        {
            QStringLiteral("nouvelles-fonctionnalites"),
            QStringLiteral("Nouvelles fonctionnalités"),
            {
                QStringLiteral("Des améliorations ont été apportées à la fiabilité de la surveillance en temps réel, au contrôle pause/reprise/arrêt et à la mise à jour du tableau sous forte charge d'événements."),
                QStringLiteral("Les instructions ci-dessous expliquent comment utiliser ces améliorations de manière sûre au quotidien."),
            },
            {
                QStringLiteral("Détection plus précise des opérations d'ajout, modification, suppression, renommage et déplacement."),
                QStringLiteral("Mise à jour visuelle plus stable sur les grandes listes, avec moins d'incohérences temporaires pendant les pics."),
                QStringLiteral("Changement de langue plus cohérent dans les menus, le tableau, le manuel, l'aide et les statistiques sans redémarrage."),
            },
            {
                {
                    QStringLiteral("Flux recommandé pour la surveillance continue"),
                    {
                        QStringLiteral("1) Ajoutez les répertoires cibles et lancez une analyse initiale complète."),
                        QStringLiteral("2) Vérifiez que le tableau est alimenté et que l'état de l'interface est correct."),
                        QStringLiteral("3) Activez la surveillance continue pour capturer les nouveaux événements en temps réel."),
                        QStringLiteral("4) Utilisez des filtres pour réduire le bruit et accélérer les audits."),
                    },
                    {}
                },
                {
                    QStringLiteral("Utilisation sûre de pause, reprise et arrêt"),
                    {
                        QStringLiteral("Pendant les opérations longues, mettez en pause pour effectuer une revue intermédiaire."),
                        QStringLiteral("Reprenez uniquement après validation du jeu de données visible."),
                        QStringLiteral("Avant de fermer, arrêtez explicitement la surveillance/l'analyse et attendez la mise à jour finale d'état."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("operation-grand-volume"),
            QStringLiteral("Exploitation en grand volume"),
            {
                QStringLiteral("Quand de nombreux fichiers changent en séquence, travaillez par cycles courts : capturer, vérifier, filtrer et exporter."),
            },
            {
                QStringLiteral("Triez par date/événement pour prioriser les changements récents."),
                QStringLiteral("Appliquez des filtres par type/répertoire avant l'export."),
                QStringLiteral("Utilisez des pauses stratégiques pour créer des points de contrôle clairs."),
            },
            {
                {
                    QStringLiteral("Changement de langue en activité intense"),
                    {
                        QStringLiteral("Le changement de langue est possible en surveillance active, mais une pause est recommandée lors de validations visuelles critiques."),
                        QStringLiteral("Après changement, vérifiez les textes des menus, en-têtes du tableau, aide et statistiques."),
                    },
                    {}
                },
                {
                    QStringLiteral("Validation rapide de stabilité"),
                    {
                        QStringLiteral("Vérifiez que de nouveaux événements apparaissent toujours après reprise de la surveillance."),
                        QStringLiteral("Confirmez que les libellés d'état et de progression changent correctement selon les états."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("ressources-complementaires-usage"),
            QStringLiteral("Ressources d'usage complémentaires"),
            {
                QStringLiteral("Cette version ajoute aussi des ressources pratiques pour l'usage quotidien : popup de sécurité configurable, classification d'événements affinée et ouverture des statistiques avec contrôle de cohérence."),
            },
            {
                QStringLiteral("Contrôles du popup de sécurité : seuil minimal, nombre maximal de lignes et intervalle entre alertes."),
                QStringLiteral("Séparation plus claire entre événements Renommé et Déplacé pour l'audit."),
                QStringLiteral("Flux statistiques plus cohérent lorsque des écritures sont encore en attente."),
            },
            {
                {
                    QStringLiteral("Configurer le popup de sécurité"),
                    {
                        QStringLiteral("Ouvrez le menu Sécurité puis la configuration du popup/des notifications."),
                        QStringLiteral("Définissez le seuil minimal de déclenchement des alertes."),
                        QStringLiteral("Réglez le nombre maximal de lignes affichées par popup."),
                        QStringLiteral("Configurez l'intervalle entre alertes puis enregistrez les préférences."),
                    },
                    {}
                },
                {
                    QStringLiteral("Interpréter Renommé vs Déplacé"),
                    {
                        QStringLiteral("Renommé : même répertoire, seul le nom change."),
                        QStringLiteral("Déplacé : changement de répertoire, avec ou sans changement de nom."),
                        QStringLiteral("Pour l'audit, filtrez par Événement et vérifiez les chemins source/destination dans le panneau de détails."),
                    },
                    {}
                },
                {
                    QStringLiteral("Ouvrir les statistiques avec des données stables"),
                    {
                        QStringLiteral("Terminez les opérations lourdes ou mettez en pause avant d'ouvrir les statistiques."),
                        QStringLiteral("Après ouverture, vérifiez l'absence d'états de traitement en attente."),
                        QStringLiteral("En cas d'écart ponctuel, fermez puis rouvrez après stabilisation du traitement."),
                    },
                    {}
                },
                {
                    QStringLiteral("Mode de rendu avancé (optionnel)"),
                    {
                        QStringLiteral("En cas de problème de compatibilité graphique, définissez LINCEU_OPENGL_MODE avant le lancement."),
                        QStringLiteral("Valeurs acceptées : auto, desktop, angle et software."),
                        QStringLiteral("Utilisez software uniquement pour le diagnostic, car les performances peuvent diminuer."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("flux-operationnel-menus-actualises"),
            QStringLiteral("Flux opérationnel des menus actualisés"),
            {
                QStringLiteral("Les menus Fichier, Configuration et Options couvrent désormais le cycle complet : choisir les répertoires, démarrer ou suspendre l'analyse, préparer le tableau, configurer les statistiques, exporter les données et consulter la sécurité."),
            },
            {
                QStringLiteral("Utilisez Fichier > Sélectionner un répertoire ou Ctrl+Shift+O avant l'analyse initiale."),
                QStringLiteral("Utilisez Fichier > Démarrer/Arrêter ou Ctrl+R pour contrôler l'analyse, et Fichier > Pause ou Ctrl+P pour créer un point de revue sans effacer l'état courant."),
                QStringLiteral("Utilisez Configuration > Filtres, Colonnes et Colonnes colorées avant l'export ou les statistiques afin de ne garder que les données pertinentes."),
                QStringLiteral("Utilisez Options > Manuel pour rouvrir ces instructions sans redémarrer l'application."),
            },
            {
                {
                    QStringLiteral("Séquence recommandée"),
                    {
                        QStringLiteral("1. Sélectionnez un répertoire et lancez l'analyse initiale."),
                        QStringLiteral("2. Réglez filtres, colonnes et couleurs pour garder visibles les données utiles."),
                        QStringLiteral("3. Ouvrez Configuration > Statistiques > Tracer les graphiques et choisissez les graphiques nécessaires à l'audit."),
                        QStringLiteral("4. Ouvrez Fichier > Statistiques ou appuyez sur Ctrl+G pour générer l'analyse visuelle."),
                        QStringLiteral("5. Exportez seulement après avoir confirmé que tableau et graphiques montrent le périmètre voulu."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("utiliser-menu-statistiques-graphiques"),
            QStringLiteral("Utiliser Configuration > Statistiques > Tracer les graphiques"),
            {
                QStringLiteral("Ce sous-menu contrôle les graphiques générés dans la fenêtre Statistiques. Les éléments cochés sont enregistrés dans statistics_charts_config.json dans le dossier persistant de l'application et réutilisés aux ouvertures suivantes."),
            },
            {
                QStringLiteral("Cochez uniquement les graphiques à générer pour réduire le temps de traitement et garder la fenêtre lisible."),
                QStringLiteral("Décochez les graphiques lourds ou inutiles pendant l'analyse d'un grand nombre d'événements."),
                QStringLiteral("Après modification, ouvrez Fichier > Statistiques ou appuyez sur Ctrl+G pour générer la fenêtre avec le jeu choisi."),
            },
            {
                {
                    QStringLiteral("Choisir les groupes de graphiques"),
                    {
                        QStringLiteral("Utilisez Camembert, Barres, Pareto et Distributions pour comparer volumes par type, taille ou opération."),
                        QStringLiteral("Utilisez Chronologie, Opérations par jour, Semaine/Heure et Heatmap pour étudier la concentration temporelle."),
                        QStringLiteral("Utilisez Sankey, Événement par répertoire et Type par répertoire pour identifier les flux entre opération, type de fichier et dossier surveillé."),
                        QStringLiteral("Utilisez Arbre de répertoires pour repérer les niveaux de dossier les plus actifs."),
                        QStringLiteral("Utilisez Scatter, Radar et Dotplot pour comparer tailles, fréquence et distribution des opérations."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("utiliser-fenetre-statistiques"),
            QStringLiteral("Utiliser la fenêtre Statistiques"),
            {
                QStringLiteral("La fenêtre Statistiques lit la base de surveillance et affiche les graphiques sélectionnés. Ouvrez-la seulement lorsque le tableau contient des données ; pendant les analyses lourdes, attendez la stabilisation de l'état avant d'actualiser."),
            },
            {
                QStringLiteral("Utilisez le panneau Sélectionner les graphiques pour afficher ou masquer des graphiques dans la session courante."),
                QStringLiteral("Utilisez Tout sélectionner pour activer tous les graphiques disponibles."),
                QStringLiteral("Utilisez Enregistrer la sélection pour exporter uniquement les graphiques cochés en PNG."),
                QStringLiteral("Utilisez Tout enregistrer pour exporter tous les graphiques générés vers un dossier horodaté."),
                QStringLiteral("Utilisez Actualiser après de nouveaux événements, des changements de filtres ou une modification du menu de sélection."),
                QStringLiteral("Masquez le panneau de sélection lorsque vous avez besoin de plus d'espace pour comparer les onglets et détails visuels."),
            },
            {}
        },
        {
            QStringLiteral("utiliser-menu-securite"),
            QStringLiteral("Utiliser Configuration > Sécurité"),
            {
                QStringLiteral("Le sous-menu Sécurité regroupe audit, sessions en temps réel, quarantaine, restauration et paquets de preuve. Utilisez-le après avoir configuré les répertoires et de préférence après une analyse initiale."),
            },
            {},
            {
                {
                    QStringLiteral("Profils et multi-répertoire"),
                    {
                        QStringLiteral("Ouvrez Configuration > Sécurité > Profils et multi-répertoire pour gérer le profil actif."),
                        QStringLiteral("Utilisez Ajouter un répertoire pour inclure un autre dossier et Supprimer la sélection pour retirer le dossier surligné."),
                        QStringLiteral("Modifiez le nom du profil puis cliquez Enregistrer le profil pour identifier l'ensemble surveillé."),
                        QStringLiteral("Cliquez Actualiser pour recharger le répertoire principal, le nombre de répertoires et le profil actif."),
                    },
                    {}
                },
                {
                    QStringLiteral("Centre d'alertes et sessions en temps réel"),
                    {
                        QStringLiteral("Ouvrez le Centre d'alertes pour examiner heure, niveau de risque, résumé, règles déclenchées, chemin, nom, session et preuve."),
                        QStringLiteral("Traitez les alertes critiques ou élevées avant les événements courants du tableau principal."),
                        QStringLiteral("Ouvrez Sessions en temps réel pour inspecter sessions récentes, total d'événements et répertoires impliqués."),
                        QStringLiteral("Utilisez Actualiser dans chaque fenêtre pendant la surveillance active pour charger de nouveaux enregistrements sans fermer le dialogue."),
                    },
                    {}
                },
                {
                    QStringLiteral("Quarantaine, restauration et preuves"),
                    {
                        QStringLiteral("Ouvrez Quarantaine et restauration pour consulter les onglets Quarantaine et Versions surveillées."),
                        QStringLiteral("Dans Quarantaine, sélectionnez une ligne et utilisez Ouvrir le dossier sélectionné pour localiser la copie isolée ; restaurez seulement après vérification de la destination."),
                        QStringLiteral("Dans Versions surveillées, sélectionnez une ligne et utilisez Restaurer la version surveillée pour récupérer une version enregistrée."),
                        QStringLiteral("Ouvrez Paquet de preuve, choisissez la destination, cliquez Exporter maintenant puis inspectez le dossier généré."),
                        QStringLiteral("Utilisez Effacer la base de sécurité seulement après export des preuves ou confirmation que l'ancien historique n'est plus nécessaire."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("utiliser-notifications-export"),
            QStringLiteral("Utiliser notifications et options d'exportation"),
            {
                QStringLiteral("Les notifications de sécurité et les options d'exportation aident à contrôler le volume d'information, la fréquence des alertes et la forme des données enregistrées."),
            },
            {
                QStringLiteral("Ouvrez Configuration > Sécurité > Notifications pour activer les résumés de pied/statut ou configurer les types de popup."),
                QStringLiteral("Dans les popups d'alertes, sessions ou quarantaine, réglez Activer ce popup, Déclencher à, Nombre maximal de lignes et Cooldown entre popups."),
                QStringLiteral("Ouvrez Configuration > Options d'exportation pour choisir si le fichier respecte les colonnes actives, filtres actifs ou lignes sélectionnées."),
                QStringLiteral("Utilisez Réinitialiser les options d'exportation pour revenir au comportement standard avant de générer CSV, TXT, JSON, XML, DB, XLSX ou paquets de preuve."),
            },
            {}
        },
        {
            QStringLiteral("vue-generale"),
            QStringLiteral("Vue d'ensemble"),
            {
            QStringLiteral("Linceu Lighthouse vous aide à inspecter, surveiller et analyser des fichiers et dossiers à travers plusieurs emplacements de votre ordinateur. Il centralise les informations des éléments et fournit des outils pour filtrer, générer des statistiques et extraire les métadonnées."),
        },
            {},
            {}
        },
        {
            QStringLiteral("ecran-principal"),
            QStringLiteral("Écran principal — Éléments"),
            {},
            {
            QStringLiteral("𝐁𝐚𝐫𝐫𝐞 𝐝'𝐨𝐮𝐭𝐢𝐥𝐬: Accès rapide à : Ajouter un répertoire, Démarrer l'analyse, Arrêter, Actualiser et Préférences."),
            QStringLiteral("𝐒é𝐥𝐞𝐜𝐭𝐞𝐮𝐫 𝐝𝐞 𝐫é𝐩𝐞𝐫𝐭𝐨𝐢𝐫𝐞𝐬: Zone pour ajouter, supprimer ou basculer les dossiers surveillés."),
            QStringLiteral("𝐓𝐚𝐛𝐥𝐞 𝐝𝐞𝐬 é𝐥é𝐦𝐞𝐧𝐭𝐬: Liste centrale avec colonnes (Nom, Type, Taille, Date, Événement) supportant tri et filtre."),
            QStringLiteral("𝐏𝐚𝐧𝐧𝐞𝐚𝐮 𝐝𝐞 𝐝é𝐭𝐚𝐢𝐥𝐬: Affiche les métadonnées et actions rapides pour l'élément sélectionné."),
            QStringLiteral("𝐌𝐞𝐬𝐬𝐚𝐠𝐞𝐬 & 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐬𝐢𝐨𝐧: Affiche notifications, journaux d'opération et barres de progression."),
            QStringLiteral("𝐒𝐭𝐚𝐭𝐢𝐬𝐭𝐢𝐪𝐮𝐞𝐬: Zone pour graphiques et rapports générés à partir des données collectées."),
        },
            {}
        },
        {
            QStringLiteral("demarrage"),
            QStringLiteral("Commencer"),
            {},
            {
            QStringLiteral("Ouvrez l'application via le raccourci ou l'exécutable."),
            QStringLiteral("Cliquez sur 𝐀𝐣𝐨𝐮𝐭𝐞𝐫 𝐮𝐧 𝐫é𝐩𝐞𝐫𝐭𝐨𝐢𝐫𝐞 et choisissez le dossier à surveiller."),
            QStringLiteral("Lancez une analyse initiale avec 𝐃é𝐦𝐚𝐫𝐫𝐞𝐫/𝐀𝐧𝐚𝐥𝐲𝐬𝐞𝐫."),
            QStringLiteral("Patientez ; les éléments apparaîtront avec les métadonnées disponibles."),
            QStringLiteral("Activez 𝐒𝐮𝐫𝐯𝐞𝐢𝐥𝐥𝐚𝐧𝐜𝐞 𝐜𝐨𝐧𝐭𝐢𝐧𝐮𝐞 pour détecter les changements automatiquement."),
        },
            {}
        },
        {
            QStringLiteral("modes-fonctionnement"),
            QStringLiteral("Modes de fonctionnement"),
            {},
            {
            QStringLiteral("𝐀𝐧𝐚𝐥𝐲𝐬𝐞 𝐮𝐧𝐢𝐪𝐮𝐞: Vérification immédiate du/des dossier(s) sélectionné(s)."),
            QStringLiteral("𝐒𝐮𝐫𝐯𝐞𝐢𝐥𝐥𝐚𝐧𝐜𝐞 𝐜𝐨𝐧𝐭𝐢𝐧𝐮𝐞: Surveillance des changements en temps réel et mise à jour automatique de la table."),
            QStringLiteral("𝐓𝐫𝐚𝐢𝐭𝐞𝐦𝐞𝐧𝐭 𝐩𝐚𝐫 𝐥𝐨𝐭𝐬: Extraction de métadonnées ou exécution d'actions sur plusieurs fichiers simultanément."),
        },
            {}
        },
        {
            QStringLiteral("comprendre-table"),
            QStringLiteral("Comprendre la table des éléments"),
            {
            QStringLiteral("Chaque colonne représente une propriété de l'élément. Utilisez le tri, les filtres et la sélection pour trouver et inspecter efficacement les éléments."),
        },
            {
            QStringLiteral("𝐍𝐨𝐦: Nom du fichier ou dossier ; double-clic pour ouvrir."),
            QStringLiteral("𝐓𝐲𝐩𝐞: Type détecté (image, vidéo, document, dossier)."),
            QStringLiteral("𝐓𝐚𝐢𝐥𝐥𝐞: Affiché en unités lisibles (KB/MB/GB)."),
            QStringLiteral("𝐃𝐚𝐭𝐞: Date de la dernière modification ou de l'événement enregistré."),
            QStringLiteral("É𝐯é𝐧𝐞𝐦𝐞𝐧𝐭: Action détectée (créé, modifié, déplacé, supprimé)."),
        },
            {}
        },
        {
            QStringLiteral("panneau-details"),
            QStringLiteral("Panneau de détails et métadonnées"),
            {
            QStringLiteral("La sélection d'un élément affiche les métadonnées pertinentes et des actions rapides (ouvrir dossier, copier le chemin, actualiser les métadonnées)."),
        },
            {
            QStringLiteral("𝐂𝐡𝐞𝐦𝐢𝐧 𝐜𝐨𝐦𝐩𝐥𝐞𝐭: Emplacement du fichier sur le disque."),
            QStringLiteral("𝐀𝐮𝐭𝐞𝐮𝐫 / 𝐂𝐫é𝐚𝐭𝐞𝐮𝐫: Si présent dans les métadonnées."),
            QStringLiteral("𝐃𝐢𝐦𝐞𝐧𝐬𝐢𝐨𝐧𝐬: Pour les images (largeur × hauteur)."),
            QStringLiteral("𝐃𝐮𝐫é𝐞: Pour les fichiers audio/vidéo."),
            QStringLiteral("𝐃é𝐛𝐢𝐭: Pour les médias, si disponible."),
            QStringLiteral("𝐏𝐞𝐫𝐦𝐢𝐬𝐬𝐢𝐨𝐧𝐬: Indique la protection ou les restrictions d'écriture."),
        },
            {}
        },
        {
            QStringLiteral("actions-fichiers"),
            QStringLiteral("Actions sur les fichiers"),
            {},
            {
            QStringLiteral("𝐎𝐮𝐯𝐫𝐢𝐫: Ouvrir avec l'application par défaut du système (double-clic ou bouton Ouvrir)."),
            QStringLiteral("𝐂𝐨𝐩𝐢𝐞𝐫 / 𝐃é𝐩𝐥𝐚𝐜𝐞𝐫: Utilisez copier/coller ou glisser-déposer pour déplacer des fichiers entre dossiers."),
            QStringLiteral("𝐑𝐞𝐧𝐨𝐦𝐦𝐞𝐫 / 𝐒𝐮𝐩𝐩𝐫𝐢𝐦𝐞𝐫: Utilisez le menu contextuel ; les actions permanentes nécessitent une confirmation."),
            QStringLiteral("𝐑𝐞𝐬𝐭𝐚𝐮𝐫𝐞𝐫: Lorsque disponible, récupérer les suppressions récentes via la fonction de restauration."),
            QStringLiteral("𝐏𝐫𝐨𝐩𝐫𝐢é𝐭é𝐬: Voir les métadonnées complètes et l'historique des événements du fichier."),
        },
            {}
        },
        {
            QStringLiteral("recherche-filtres"),
            QStringLiteral("Recherche et filtres"),
            {},
            {
            QStringLiteral("𝐑𝐞𝐜𝐡𝐞𝐫𝐜𝐡𝐞: Utilisez la zone de recherche pour trouver des éléments par nom ou terme."),
            QStringLiteral("𝐅𝐢𝐥𝐭𝐫𝐞𝐬: Appliquez des filtres par type, taille, date ou événement pour affiner les résultats."),
            QStringLiteral("𝐂𝐨𝐦𝐛𝐢𝐧𝐞𝐫 𝐥𝐞𝐬 𝐟𝐢𝐥𝐭𝐫𝐞𝐬: Combinez plusieurs filtres pour des résultats précis."),
        },
            {}
        },
        {
            QStringLiteral("sauvegarder-restaurer"),
            QStringLiteral("Sauvegarder et restaurer la vue"),
            {},
            {
            QStringLiteral("𝐒𝐚𝐮𝐯𝐞𝐠𝐚𝐫𝐝𝐞𝐫 𝐥'é𝐭𝐚𝐭: Enregistrez la disposition actuelle des colonnes et les filtres comme profil."),
            QStringLiteral("𝐑𝐞𝐬𝐭𝐚𝐮𝐫𝐞𝐫 𝐥'é𝐭𝐚𝐭: Chargez un profil enregistré pour retrouver la disposition et les filtres."),
        },
            {}
        },
        {
            QStringLiteral("traitement-lots"),
            QStringLiteral("Traitement par lots"),
            {
            QStringLiteral("Sélectionnez plusieurs éléments pour extraire des métadonnées ou effectuer des actions par lots. La progression et les contrôles permettent de mettre en pause ou d'annuler."),
        },
            {
            QStringLiteral("𝐃é𝐦𝐚𝐫𝐫𝐞𝐫 𝐥'𝐞𝐱𝐭𝐫𝐚𝐜𝐭𝐢𝐨𝐧: Choisissez des éléments et lancez l'extraction des métadonnées par lot."),
            QStringLiteral("𝐒𝐮𝐫𝐯𝐞𝐢𝐥𝐥𝐞𝐫 𝐥𝐚 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐬𝐢𝐨𝐧: Suivez via la barre de progression et le panneau de messages."),
            QStringLiteral("𝐏𝐚𝐮𝐬𝐞/𝐀𝐧𝐧𝐮𝐥𝐞𝐫: Utilisez les contrôles pour interrompre le traitement."),
        },
            {}
        },
        {
            QStringLiteral("rapports-statistiques"),
            QStringLiteral("Rapports et statistiques"),
            {
            QStringLiteral("La section statistiques produit des graphiques sur les types de fichiers, l'utilisation du disque et les tendances d'événements. Exportez images ou CSV pour une analyse externe."),
        },
            {
            QStringLiteral("𝐆é𝐧é𝐫𝐞𝐫 𝐝𝐞𝐬 𝐠𝐫𝐚𝐩𝐡𝐢𝐪𝐮𝐞𝐬: Sélectionnez des métriques pour visualiser des distributions et tendances."),
            QStringLiteral("𝐄𝐱𝐩𝐨𝐫𝐭𝐞𝐫: Enregistrez les graphiques en images ou exportez les données en CSV."),
        },
            {}
        },
        {
            QStringLiteral("monitorage-evenements"),
            QStringLiteral("Surveillance des événements"),
            {},
            {
            QStringLiteral("𝐓𝐲𝐩𝐞𝐬 𝐝'é𝐯é𝐧𝐞𝐦𝐞𝐧𝐭𝐬: Création, modification, suppression et déplacement."),
            QStringLiteral("𝐕𝐮𝐞 𝐭𝐞𝐦𝐩𝐬 𝐫é𝐞𝐥: Les événements récents sont mis en évidence pour une inspection rapide."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐚𝐭𝐢𝐨𝐧𝐬: Configurez des alertes visuelles ou sonores dans Préférences."),
        },
            {}
        },
        {
            QStringLiteral("preferences-fr"),
            QStringLiteral("Préférences"),
            {
            QStringLiteral("Ajustez la langue, le comportement d'analyse, les notifications et les mises à jour. Vérifiez ces options après l'installation."),
        },
            {
            QStringLiteral("𝐋𝐚𝐧𝐠𝐮𝐞: Choisissez la langue de l'interface."),
            QStringLiteral("𝐀𝐧𝐚𝐥𝐲𝐬𝐞: Configurez la fréquence, la profondeur et les exclusions."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐚𝐭𝐢𝐨𝐧𝐬: Activez ou désactivez les alertes."),
            QStringLiteral("𝐌𝐢𝐬𝐞𝐬 à 𝐣𝐨𝐮𝐫: Choisissez vérification automatique ou manuelle."),
        },
            {}
        },
        {
            QStringLiteral("problemes-communs-fr"),
            QStringLiteral("Problèmes courants et solutions"),
            {},
            {
            QStringLiteral("𝐀𝐧𝐚𝐥𝐲𝐬𝐞 𝐛𝐥𝐨𝐪𝐮é𝐞 𝐨𝐮 𝐞𝐫𝐫𝐞𝐮𝐫: Patientez ; si cela persiste, redémarrez l'analyse ou l'application."),
            QStringLiteral("É𝐥é𝐦𝐞𝐧𝐭𝐬 𝐦𝐚𝐧𝐪𝐮𝐚𝐧𝐭𝐬: Vérifiez que le dossier a été ajouté et que l'application dispose des droits de lecture."),
            QStringLiteral("𝐈𝐦𝐩𝐨𝐬𝐬𝐢𝐛𝐥𝐞 𝐝'𝐨𝐮𝐯𝐫𝐢𝐫: Assurez-vous qu'une application est associée au type de fichier."),
            QStringLiteral("𝐌é𝐭𝐚𝐝𝐨𝐧𝐧é𝐞𝐬 𝐦𝐚𝐧𝐪𝐮𝐚𝐧𝐭𝐞𝐬: Tous les fichiers n'incluent pas de métadonnées ; essayez d'actualiser ou de traiter par lots."),
        },
            {}
        },
        {
            QStringLiteral("raccourcis-fr"),
            QStringLiteral("Raccourcis"),
            {},
            shortcut_lines(QStringLiteral("fr_FR")),
            {}
        },
        {
            QStringLiteral("bonnes-pratiques-fr"),
            QStringLiteral("Bonnes pratiques"),
            {},
            {
            QStringLiteral("Effectuez une première analyse sur un petit dossier avant les opérations volumineuses."),
            QStringLiteral("Sauvegardez états et configurations avant les traitements par lots."),
            QStringLiteral("Utilisez des filtres pour vous concentrer sur les éléments pertinents."),
        },
            {}
        },
        {
            QStringLiteral("logs-diagnostic-fr"),
            QStringLiteral("Journaux et diagnostic"),
            {},
            {
            QStringLiteral("Les journaux de l'application peuvent aider à diagnostiquer les problèmes ; consultez le fichier journal généré par l'application ici : ") + dataDir,
        },
            {}
        },
        {
            QStringLiteral("faq-fr"),
            QStringLiteral("Questions fréquentes (FAQ)"),
            {},
            {},
            {
        {
            QStringLiteral("Où mes données sont-elles enregistrées ?"),
            {
            QStringLiteral("Les tâches et fichiers de configuration sont stockés dans : ") + dataDir,
            QStringLiteral("Consultez ce répertoire pour trouver les fichiers de persistance et les journaux."),
        },
            {}
        },
    }
        },
        {
            QStringLiteral("support-fr"),
            QStringLiteral("Comment obtenir de l'aide et du support"),
            {},
            {
            QStringLiteral("Consultez la section À propos dans l'application pour les informations officielles et les notes de version."),
            QStringLiteral("Pour les problèmes complexes, générez des journaux et envoyez-les au support. Les fichiers journaux se trouvent ici : ") + dataDir,
            QStringLiteral("Envoyez les journaux et une description détaillée du problème à l'email de support : linceu_lighthouse@outlook.com."),
        },
            {}
        },
    };
}

}  // namespace source::ui::manual
