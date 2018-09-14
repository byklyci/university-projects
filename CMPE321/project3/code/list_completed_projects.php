<html>
<head>
</head>
<body>
    <?php 
        $servername = "localhost";
        $username = "root";
        $password = "";
        $dbname = "cmpe321hw3";

        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);

        // Check connection
        if ($conn->connect_error) {

            die("Connection failed: " . $conn->connect_error);
        }else{
            session_start();

            if($_SESSION['AdminRole']==1){
                $sql = "SELECT ID FROM user WHERE AdminRole = 0";
                $result = $conn->query($sql);
    
                if ($result->num_rows > 0) {
    
                    ?>
                    <form action="list_completed_projects_result.php" method="post">
                        <p>Project Manager ID: 
                        <?php 
                            $select= '<select name="ProjectManagerID">';
                            while($row = $result->fetch_assoc()){
                                $select.='<option value="'.$row['ID'].'">'.$row['ID'].'</option>';
                            }
                            $select.='<option value="ALL">ALL</option>';
                            $select.='</select>';
                            echo $select;
                        ?>
                        </p>
                        <p><input type="submit" value = "See completed projects"/></p>
                    </form>
    
                    <?php
                } else {
                    echo "The table is empty.";
                }
            }else{
                
            }
            echo "<p><a href = 'index.php'>Go back</a></p>";
        }
        $conn->close();
    ?>
</body>
</html>