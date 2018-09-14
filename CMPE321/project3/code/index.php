<html>
<head>
</head>
<body>
    <?php 
        session_start();

        if(!isset($_SESSION['username'])){
            header("Location: login.php");
        }else{
            if($_SESSION['AdminRole']==1){
                header("Location: admin_panel.php");
            }else{
                header("Location: project_manager_panel.php");
            }
        }
    ?>
</body>
</html>