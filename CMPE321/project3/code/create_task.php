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
            $sql = "SELECT ProjectID FROM manageproject WHERE ProjectManagerID = " . $_SESSION['ID'];
            $result = $conn->query($sql);

            if ($result->num_rows > 0) {

                ?>
                <form action="create_task_result.php" method="post">
                    <p>Project ID: 
                    <?php 
                        $select= '<select name="ProjectID">';
                        while($row = $result->fetch_assoc()){
                            $select.='<option value="'.$row['ProjectID'].'">'.$row['ProjectID'].'</option>';
                        }
                        $select.='</select>';
                        echo $select;
                    ?>
                    </p>
                    <p>Start Date: <input type="text" name="StartDate" value = "" placeholder="e.g. 2018-05-27" /></p>
                    <p>Day It Takes: <input type="text" name="DayItTakes" value = "" /></p>
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
