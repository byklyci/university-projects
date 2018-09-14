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
            $sql = "SELECT ID as TaskID FROM task WHERE ProjectID IN (SELECT ProjectID FROM manageproject WHERE ProjectManagerID = " . $_SESSION['ID']. ")";
            $result = $conn->query($sql);

            if ($result->num_rows > 0) {

                ?>
                <form action="create_dotask_result.php" method="post">
                    <p>Task ID: 
                    <?php 
                        $select= '<select name="TaskID">';
                        while($row = $result->fetch_assoc()){
                            $select.='<option value="'.$row['TaskID'].'">'.$row['TaskID'].'</option>';
                        }
                        $select.='</select>';
                        echo $select;
                    ?>
                    </p>
                    <p>Employee ID: <input type="text" name="EmployeeID" value = "" /></p>
                    <p><input type="submit" value = "Create Record"/></p>
                </form>

                <?php
            } else {
                echo "You do not have any projects assigned to you.";
            }
        }
        $conn->close();
    ?>

</body>
</html>
