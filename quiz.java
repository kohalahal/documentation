package Practice;
import java.util.*;


public class quiz {

			public static void main(String[] args) {
				Scanner scanner = new Scanner(System.in);
				
				System.out.println("Enter my Korean age->");
				String input = scanner.nextLine();
				int num = Integer.parseInt(input);
				System.out.println("You entered:"+num);


				int answer = 31;
				boolean correct = num == answer;
				System.out.println(correct);
				
				
				if (num == 31) {
				System.out.println("genius");
				} else {
				System.out.println("^^???");
				}
		
		
			}
}

